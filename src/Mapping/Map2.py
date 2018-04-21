import math

#Current Coordinates of the robot.
# Angle is kept between 180 and -180
Current_Coord = {'x': 0, 'y': 0, 'ang': 90}

#global used by functions
prevDistance = 0

outerParameter = []
patrolPath = []

#State of the Robot
mode = ["Mapping", "Manual", "Patrol"];
currentMode = "Manual";

# Class created for a list of points.
class point:
    #creates the class of x,y points.
    def __init__(self,x,y):
        self.x = x
        self.y = y


# Function call which updates Current_Coord
# Left angle == POSITIVE, Right angle == NEGATIVE
# assumes Arduino gives a continuous distance while it's going 1
# direction, otherwise returns 0. Angle gives updated angle.
def updateCurrCordinate(distance, angle):
    changeInDist = 0
    global prevDistance

    if distance != 0:
        changeInDist = distance - prevDistance
    prevDistance = distance

    #keeps angle between 180 and -180
    Current_Coord['ang'] += angle
    if Current_Coord['ang'] > 180:
        Current_Coord['ang'] -= 360
    if Current_Coord['ang'] < -180:
        Current_Coord['ang'] += 360

    Current_Coord['x'] += changeInDist * math.cos(Current_Coord['ang'])
    Current_Coord['y'] += changeInDist * math.sin(Current_Coord['ang'])


# Defining Mapping State;
# Function for the mapping; will only add corner points to the array
# **Unsure how the distance and angle will be updated from the robot.
def mappingMode():
    isAngleChanged = 0
    pastAngle = Current_Coord['ang']
    resetStates()

    #temp
    angle = 0

    while(currentMode == mode[0]):
        updateCurrCordinate(distance, angle)
        # needs a way to keep track if the distance and the angle has changed!!

        #if angle is changed the first time; add onto the trace.
        if angle != pastAngle:
            if not isAngleChanged:
                addCurrToParameter()
                isAngleChanged = 1
            pastAngle = angle

        # Has stopped rotating and back to moving.
        if isAngleChanged and distance != 0:
            isAngleChanged = 0

    # when it exists the while loop == the bot has stopped. adding stopping point
    # into the list
    addCurrToParameter()


# helper function to reset the current coordinates and parameter array.
def resetStates():
    global Current_Coord
    global outerParameter
    Current_Coord = {'x': 0, 'y': 0, 'ang': 90}
    outerParameter = []

# Helper function to add current location to the parameter array
def addCurrToParameter():
    global outerParameter
    outerParameter.append(point(Current_Coord['x'],Current_Coord['y']))


# Function call which tells the robot to go to a certain X,Y coordinate.
def motor_control(x, y, serial_port):

    # calculates the angle based on the difference of the current location and the
    # destination X,Y

    deltaX = x - Current_Coord[x]
    deltaY = y - Current_Coord[y]
    #calculates angle
    destinationAngle = math.degrees(math.atan2(deltaY/deltaX))
    angleToChange = destinationAngle - Current_Coord['ang']

    robot_control("turn", angleToChange, serial_port)

    distance = ((deltaX) ** 2 + (deltaY) ** 2) ** (1 / 2)

    robot_control('forward', distance, serial_port)

    return 1

# helper function to detect distance between 2 points
def distance(x1, y1, x2, y2):
    deltaX = x2 - x1
    deltaY = y2 - y1
    return ((deltaX) ** 2 + (deltaY) ** 2) ** (1 / 2)


# prototype mode for patrol
# Will go directly through the array of points given by user.
def patrolMode():
    numOfCheckpoints = len(patrolPath)
    maxCheckpoints = (len(patrolPath) * 2) - 1
    currentIter = -1;

    while currentMode == mode[2]:
        # shouldn't be 0; gets out of patrol mode
        if numOfCheckpoints == 0:
            break
        # if it's not within range of the destination or in the first iteration

        #if distance(patrolPath[currentIter].x, patrolPath[currentIter].y, Current_Coord['x'], Current_Coord['y']) > 1 \
        #and currentIter != -1:
        #    pass

        # **NEEDS A CHECK TO SEE IF THE ROBOT HAS STOPPED**
        # WILL RETRY IF IT HAS STOPPED

        #elif distance(patrolPath[currentIter].x, patrolPath[currentIter].y, Current_Coord[x], Current_Coord[y]) > 1 \
        #and currentIter != -1 and (HAS STOPPED):
        #    if currentIter > numOfCheckpoints-1:
        #        tempIter = currentIter - ((currentIter + 1) % numOfCheckpoints)
        #        motor_control(patrolPath[tempIter].x , patrolPath[tempIter].y ,serial_port)
        #    else:
        #        motor_control(patrolPath[currentIter].x , patrolPath[currentIter].y ,serial_port)


        elif currentIter > numOfCheckpoints-1:
            currentIter += 1
            if currentIter > maxCheckpoints:
                currentIter = 0
            tempIter = currentIter - ((currentIter + 1) % numOfCheckpoints)
            motor_control(patrolPath[tempIter].x , patrolPath[tempIter].y ,serial_port)
        else:
            currentIter += 1
            if currentIter > maxCheckpoints:
                currentIter = 0
            motor_control(patrolPath[currentIter].x , patrolPath[currentIter].y ,serial_port)
