"""
This is the program that will connect the arduino on the car with the server
It has some requirements:
Author: Ming-han Ko, Yiyi Yan
1. The server has to follow the request format from http://griin.today/
2. The arduino has to follow the protocol in order to communicate with this program
    It has to connect via one of the three ports: COM13, COM15 or '/dev/ttyACM0'
3. This program should be running in an individual process on rasspberry pi 3

Protocal between arduino and pi are as follow:
"""
import math
from time import sleep
import serial
import queue
import re
import requests
import json


class Coordinate:
    """
    creates the class of x,y, theta coordinate of the robot
    """

    def __init__(self, x=0, y=0, theta=90):
        self.x = x
        self.y = y
        self.theta = theta

    def print_coordinate(self):
        """
        This function prints the current coordinate for debugging purpose
        :return:
        """
        print("coordinate: x: {0}, y: {1}, theta: {2}".format(self.x, self.y, self.theta))

    def return_pos_to_dict(self):
        """
        :return: x and y
        """

        return {"x": self.x, "y": self.y}

    def return_coordinate_to_dict(self):
        """
        :return: json files of the x, y and angle
        """
        return {"x": self.x, "y": self.y, "angle": self.theta}


# Current Coordinates of the robot.
# Angle is kept between 0-180 and -180 to 0, following the converition of atan2:
# counterclockwise start from 0 v(1, 0), to 90 degree at v(0, 1) and 180 at v(-1, 0), then as
# -180 at v(-1, 0) to 0 at v(1, 0)
current_coord = Coordinate()

# This is saved from the server
patrol_path = []

# The three mode of what we are doing
mode = {"Manual": 0, "Mapping": 1, "Patrol": 2}

# The default mode of the program is 0
current_mode = 0

# global serial port
ser = serial

# index for patrolling path, to indeicate the next point to patrol
current_patrol_point = 0

# the mapping data storage
boundary_map = queue.Queue()

# Debugging bits that disable and enable functionalities
DEBUG = 0

ARDUINO = 0

NO_CAR = 0

MODE_1_DISABLED = 1

MODE_2_DISABLED = 1

# Prototyping variables that were never ussed
map_point = False

revert = False


# helper function to reset the current coordinates and parameter array.
def resetStates():
    """
    This function reset every storage data, only for mapping mode
    :return:
    """
    global current_coord
    global boundary_map
    global serial_message

    current_coord = Coordinate()
    boundary_map = queue.Queue(maxsize=1000)
    boundary_map.put(current_coord)
    serial_message = queue.Queue()


# Function call which tells the robot to go to a certain X,Y coordinate.
def motor_control(x, y):
    """
    This is the function will control the motor based on x and y
    :param x:
    :param y:
    :return:
    """
    # calculates the angle based on the difference of the current location and the
    # destination X,Y
    global ser
    print("Driver: going to x: {0}, y: {1}".format(x, y))
    deltaX = x - current_coord.x
    deltaY = y - current_coord.y
    if deltaX == 0 and deltaY == 0:
        print("Robot Arrive at the position")
        return
    # calculates angle
    destinationAngle = math.degrees(math.atan2(deltaY, deltaX))
    angleToChange = destinationAngle - current_coord.theta

    # trim the angle to fit the calculation
    if angleToChange > 180:
        angleToChange -= 360
    if angleToChange < -180:
        angleToChange += 360

    # calculate the distance
    delta_distance = (deltaX ** 2 + deltaY ** 2) ** (1 / 2)

    # update current coordinate one by one.
    # each data is depended on each other, so we have to modify them one by one.
    update_current_coordinate(0, angleToChange)
    update_current_coordinate(delta_distance, 0)

    # if we are testing without the car, we jusst sleep and print message
    if NO_CAR == 1:
        sleep(2)
        print("Arrived at x: {0}, y: {1}".format(x, y))
        return

    # turn the car with delta angle
    robot_control("turn", angleToChange)

    # go forward when there is a distance
    robot_control('forward', delta_distance)

    # prints message to indicate the completion of the car control
    print("Driver: Arrived at x: {0}, y: {1}".format(x, y))


# this module only send data
# Thi function takes
def robot_control(command, arg1):
    """
    This iss the lowest layer of the program, it directly controls the car.
    :param command:
    :param arg1:
    :return: nothing
    """
    # process the direction
    # process the time (if any)
    global message
    global current_mode
    global ser
    # ADding 0 at the front to work with patrol mode and maual mode

    if command == 'forward':
        message = "01" + ("1" if arg1 >= 0 else "0") + str(abs(arg1)).zfill(4)
    elif command == 'turn':
        message = "02" + ("0" if arg1 >= 0 else "1") + str(abs(arg1)).zfill(4)
    elif command == 'stop':
        # there are two way to stop: one is natural one is interrupt stop/
        message = "03" + str(0).zfill(5)

    ser.write(message.encode())

    # wait for arduino feedback to send another serial message

    # TODO The program can get Stucked here!
    # solved: only limit the serial port read 1 per mode, otherwise it will miss it.
    while ser.readline() != b'done\r\n':
        if current_mode == mode['Mapping']:
            break
        print("Waiting for feedback")

    print('Robot control message sent!')
    return


def make_request(URL, http_mode

='GET', data=None):
    """
    THis function get or post request to the server.
    :param URL:
    :param http_mode:
    :param data:
    :return: directionarry of the data
    """
    if http_mode == 'GET':
        r = requests.get(URL)
        return json.loads(r.content.decode("utf-8"))
    else:
        requests.post(URL, data)
        return


def portIsUsable(portName):
    """
    Check if the port is avaible
    :param portName:
    :return: boolean for is the port is connected or not
    """
    try:
        serial.Serial(port=portName, timeout=10, baudrate=9600)
        return True
    except:
        return False


def serial_message_parser(one_message):
    """
    The format of the output message is:
    forward arg1(-9999 < arg1 < 9999)
    turn arg1 (-90 < arg1 < 90)
    Distance: < int > Angle: < int >
    :param one_message:
    :return:
    """
    received_coordinate = {}

    # parse the float data from the
    data = re.findall(r"[-+]?\d*\.\d+|\d+", str(one_message))
    # IF the data is corrupted
    if len(data) != 2:
        received_coordinate['cmd'] = "forward"
        received_coordinate['arg1'] = 0
        return received_coordinate

    # if the daa is good
    if float(data[0]) != 0:
        received_coordinate['cmd'] = "forward"
        received_coordinate['arg1'] = float(data[0])
    else:
        received_coordinate['cmd'] = "turn"
        received_coordinate['arg1'] = float(data[1])

    return received_coordinate


# helper function to detect distance between 2 points
def distance(x1, y1, x2, y2):
    """
    get the disstance between two points
    :param x1:
    :param y1:
    :param x2:
    :param y2:
    :return:
    """
    deltaX = x2 - x1
    deltaY = y2 - y1
    return (deltaX ** 2 + deltaY ** 2) ** (1 / 2)


def update_current_coordinate(delta_distance, delta_angle):
    """
    Function call which updates Current_Coord
    Left angle == POSITIVE, Right angle == NEGATIVE
    assumes Arduino gives a continuous distance while it's going 1
    direction, otherwise returns 0. Angle gives updated angle.
    :param delta_angle:
    :param delta_distance:
    :return:
    """
    global current_coord

    # the current heading is where the distance headed, so we update the angle in the end
    # change the precision of the coordinates
    current_coord.x += round((delta_distance * math.cos(math.radians(current_coord.theta))), 2)
    current_coord.y += round((delta_distance * math.sin(math.radians(current_coord.theta))), 2)

    # avoid inputs that are larger than 360
    delta_angle = delta_angle % 360

    # keeps angle between 0 and 360
    current_coord.theta += round(delta_angle, 2)

    # clamp theat between -180 and 180 for the atan2
    if current_coord.theta > 180:
        current_coord.theta -= 360
    if current_coord.theta < -180:
        current_coord.theta += 360
    return

"""
This part are the code for modes
Contains: 
mapping mode
patrol mode
manual mode
"""


# Defining Mapping State;
# Function for the mapping; will only add corner points to the array
# **Unsure how the distance and angle will be updated from the robot.
def mapping_mode():
    # de queue
    """
    # :return:
    # False if dumped the serial message and add a point with no turn
    # True of add a way point with angle change
    # """

    global boundary_map
    global map_point

    # local variables
    delta_distance = 0
    delta_angle = 0

    # read message from serial port, this should be a dictionary
    parsed_message = serial_message_parser(ser.readline().decode("utf-8"))

    print(parsed_message)
    current_coord.print_coordinate()

    # add data based on its types
    if parsed_message['cmd'] == "forward":
        delta_distance = parsed_message['arg1']
    if parsed_message['cmd'] == "turn":
        delta_angle = parsed_message['arg1']
    # update the coordinate whenever there is a reading
    update_current_coordinate(delta_distance, delta_angle)

    # if the angle changed, we add the point to the boundary map and push the point to the
    # server
    if delta_angle != 0:
            boundary_map.put(current_coord)
            print("corrdinate send")
            make_request("http://griin.today/API/boundaries", "POST", current_coord.return_pos_to_dict())
    return


def patrol_mode():
    """
     The following are codes for threads
     this loop will keep looping, until
    """
    # if the queue is updated
    global current_patrol_point
    global revert

    # avoid runing patrol mode if the path is not loaded
    if len(patrol_path) == 0:
        print("Map Needed or No patrol path loaded")
        return

    print("Patrol point %d" % current_patrol_point)

    # patrol to points
    motor_control(patrol_path[current_patrol_point]['x'], patrol_path[current_patrol_point]['y'])

    # make sure we can travel back and forth on the patrol path
    if revert:
        current_patrol_point -= 1
        if current_patrol_point < 0:
            current_patrol_point = 1
            revert = False
    else:
        current_patrol_point += 1
        if current_patrol_point >= len(patrol_path):
            current_patrol_point = len(patrol_path) - 2
            revert = True
    return


"""
Thread functions
"""


def robot_mode_main():
    """
    This is the main logic of the program. This function is iterated as fast as possible,
    getting updates in the mode from the server and initialize or continue the action based
    on the mode returned
    :return:
    """
    global current_mode
    global ser
    global current_patrol_point
    global boundary_maps
    global patrol_path
    global revert

    while True:
        if DEBUG != 1:
            # get the current mode at every iteration
            temp = make_request("http://griin.today/API/current_mode", "GET")['current_mode']

            # update the current coordinate
            if temp != current_mode:
                change = 1
            else:
                change = 0

            current_mode = temp
        else:
            current_mode = 1
            change = 0

        print("Current Mode: " + str(current_mode))

        """
        This is the state for mapping mode
        The entry point is the to reset the state of the robot(starting at x=0, y=0, theta=90)
        """
        if current_mode == mode['Mapping']:
            if change == 1:
                resetStates()
                # send the mapping starting mode to the robot
                ser.write("1000000".encode())

                print("--------------initialized mapping!")

                # send the points to the website
                make_request("http://griin.today/API/boundaries", "POST", {"x": 0, "y": 0})

            # running the mapping mode for every iteration
            mapping_mode()
        elif current_mode == mode['Patrol']:
            """
            This is the state machine for patrol mode, where each functions. 
            The entry point is to get the patrol path from the server, and sent the current point 
            to patrol to 0. 
            """
            if change == 1:
                if DEBUG != 1:
                    global patrol_path
                    new_path = make_request("http://griin.today/API/patrol_path", "GET")['patrol_path']

                    # if the len is not the same, that means it's the new boundary
                    # If the new has different length(a lazy way to check, since we pull patrol path from the server
                    # all the time and if this is slow, it will introduce more lag into the patrolling)
                    if len(new_path) != len(patrol_path):
                        patrol_path = new_path
                        current_patrol_point = 0
                        revert = False

                else:
                    patrol_path = {}

            patrol_mode()

            current_coord.print_coordinate()

            # Send the current location to the server in form of the dictionary
            make_request("http://griin.today/API/current_location", "POST", current_coord.return_coordinate_to_dict())

        elif current_mode == mode['Manual']:
            """
            The state machine of the manual mode, at the initial state it sends the current location to the server 
            to update the location
            """
            if change == 1:
                make_request("http://griin.today/API/current_location", "POST",
                             current_coord.return_coordinate_to_dict())
                print("-------------initial Manual Mode:")
                current_coord.print_coordinate()

            """
            The default data is set to x = null, y = null. Therefore we need to chec if 
            the location is valid
            """
            if DEBUG != 1:
                location = make_request("http://griin.today/API/current_target", "GET")
                print(location)
                try:
                    # check if the data is valid or not
                    float(location['x'])
                    float(location['y'])
                except:
                    print("Invalid Location")
                    continue
            else:
                location = {"x": 0, "y": 0}

            print("Going to: " + str(int(location['x'])) + " " + str(int(location['y'])))

            # Since we have some calculation error, we need to avoid trying to get to the same point
            # again due to the slight difference, so we set a thresh hold to make sure the go to
            # location state terminates after within 3 cm radius within the target
            if abs(location['x'] - current_coord.x) < 3 and abs(location['y'] - current_coord.y) < 3:
                pass
            else:
                current_coord.print_coordinate()
                motor_control(float(location['x']), float(location['y']))
                current_coord.print_coordinate()

            # Send the current location to the server to update it on the website
            make_request("http://griin.today/API/current_location", "POST", current_coord.return_coordinate_to_dict())
            current_coord.print_coordinate()
        else:
            assert "Mode is not defined!"


if __name__ == "__main__":
    """
    
    """
    print("Program init")

    # Since we don't know what port we will connect to, we just need to try all three ports that we usually use
    port = 'COM13'

    while not portIsUsable(port):
        print("Try to connect to " + port)
        sleep(2)
        if port == 'COM13':
            port = 'COM15'
        else:
            # the port name for pi
            port = '/dev/ttyACM0'

    # connect to the port with timeout setting to 10
    ser = serial.Serial(port=port, timeout=10, baudrate=9600)

    print("Arduino connected")
    # TODO: the hand shake process to make sure it's connected
    sleep(2)
    print("Start controlling")
    print("Debug mode is " + str(DEBUG))
    print("Arduino mode is " + str(ARDUINO))
    print("No car mode is " + str(NO_CAR))

    robot_mode_main()

    # this loop traps the process. We tried to rerun the main when there is a exception(connection problem, or
    # an arduino reading error), but it introduced problems when there are error that reset the program, and cause
    # the program to lose the connection, and get stucked in the infinite loop above. So we decided to simply restart
    # the whole program once the program failed
    while True:
        pass
