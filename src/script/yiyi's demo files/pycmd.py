from threading import Thread
from time import sleep
import serial

import sys
import glob


# this module only send data
# Thi function takes
def robot_control(command, arg1, serial_port):
    # process the direction

    # process the time (if any)
    global message
    if command == 'forward':
        message = "1" + ("1" if arg1 >= 0 else "0") + str(abs(arg1)).zfill(4)
    elif command == 'turn':
        message = "2" + ("1" if arg1 >= 0 else "0") + str(abs(arg1)).zfill(4)
    elif command == 'stop':
        # there are two way to stop: one is natural one is interrupt stop/
        message = "3" + str(0).zfill(5)

    #     import RPi.GPIO as GPIO
    #     GPIO.setmode(GPIO.BCM)
    #     GPIO.setwarnings(False)
    #     GPIO.setup(18, GPIO.OUT)
    #     GPIO.output(18, GPIO.HIGH)
    #
    #     while serial_port.readline() != b'stopped\r\n':
    #         print("Waiting for stop\n")
    #         pass
    #     GPIO.output(18, GPIO.LOW)
    #     return 0
    # else:
    #     print("Invalid messages\n")
    #     return -1


    # only forward and turn will reach here
    serial_port.write(message.encode())

    print(message)

    # wait for arduino feedback to send another serial message
    while serial_port.readline() != b'done\r\n':
        pass

    print('robot control message sent!')
    return 1


def robot_motor_turn(angle):
    print("Robot turn %d angle", angle)
    # robot_control("turn", angle)
    return 1


def robot_motor_forward(distance):
    print("Robot go forward %d distance", distance)
    # robot_control("forward", distance)
    return -1


# the absoulte position and angling
def motor_control(x, y, theta, serial_port):
    robot_control("turn", theta - current_state[theta], serial_port)
    # calculate
    distance = ((x - current_state[x]) ** 2 + (y - current_state[y]) ** 2) ** (1 / 2)

    # perfer to cut the distance to piece, so that we can run the obstacle avoidence at the same time.
    robot_control('forward', distance, serial_port)

    return 1

# encode image
def make_request_image(URL, image, wait=False):
    sleep(4)
    print("image request send!")
    return


# url string, data string
def make_request(URL, data, wait=False):
    # #Inner
    # URL = "/feed/1"
    # URL = "38.34.23.34" + URL
    # #json
    # data = '{"id": 2, "inner object": {"id": 5}}'
    # jsonify(data)
    # http.GET(URL, data)
    # #Inner
    sleep(2)
    print("data request send! %s", data)
    return


# this loop will keep looping, until
def main_loop():
    pass

def robot_position_update():
    pass

# Check port
def portIsUsable(portName):
    try:
        serial.Serial(port=portName, timeout=10, baudrate=9600)
        return True
    except:
        return False



# the current state of the robot
# if we use accel and gyro, we will get this from them.
current_state = {'x': 0, 'y': 0, 'theta': 0}




if __name__ == "__main__":
    print("Program init")

    port = 'COM15'
    while not portIsUsable(port):
        print("Try to connect to " + port)
        sleep(2)
    ser = serial.Serial(port=port, timeout=10, baudrate=9600)

    # TODO: the hand shake process to make sure it's connected
    print("Start controlling")

    robot_control('forward', 10, ser)
    print("Forwarded")
    sleep(10)
    robot_control('turn', 90, ser)
    print("Left")
    sleep(2)
    robot_control('turn', -90, ser)
    print("right")
    sleep(2)

    ser.close()
    print("Closed " + port)


    # # the first thread is the main one that will send
    # main_thread = Thread(target=main_loop, args=())
    # motor_thread = Thread(target=robot_control, args=())
    # robot_odometry = Thread(target=robot_position_update, args=())
    #
    # main_thread.start()
    # motor_thread.start()
    # robot_odometry.start()
    #
    # main_thread.join()
    # motor_thread.join()
    # robot_odometry.join()
    #
    # serial_port.close()
