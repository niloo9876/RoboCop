# copied online to check the serial port
def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result


    # isAngleChanged = True
    # pastAngle = Current_Coord['ang']
    # resetStates()
    # # temp
    # angle = 0
    # while current_mode == mode['Mapping']:
    #     updateCurrCordinate(distance, angle)
    #     # needs a way to keep track if the distance and the angle has changed!!
    #     # if angle is changed the first time; add onto the trace.
    #     if angle != pastAngle:
    #         if not isAngleChanged:
    #             addCurrToParameter()
    #         isAngleChanged = 1
    #         pastAngle = angle
    #     # Has stopped rotating and back to moving.
    #     if isAngleChanged and distance != 0:
    #         isAngleChanged = 0
    # # when it exists the while loop == the bot has stopped. adding stopping point
    # # into the list
    # addCurrToParameter()

def patrolMode():
    numOfCheckpoints = len(patrolPath)
    maxCheckpoints = (len(patrolPath) * 2) - 1
    currentIter = -1

    while current_mode == mode[2]:
        # shouldn't be 0; gets out of patrol mode
        if numOfCheckpoints == 0:
            return
        elif currentIter > numOfCheckpoints - 1:
            currentIter += 1
            tempIter = currentIter - ((currentIter + 1) % numOfCheckpoints)
            motor_control(patrolPath[tempIter].x, patrolPath[tempIter].y, ser)
        else:
            currentIter += 1
            motor_control(patrolPath[currentIter].x, patrolPath[currentIter].y, ser)

