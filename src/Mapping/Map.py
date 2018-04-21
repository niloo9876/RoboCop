import math

#Current Coordinates of the robot.
currCord_x = 0
currCord_y = 0
currAngle = 90

#global used by functions
prevDistance = 0
obstacles =[]




#updates currentCoordinates
#if right turning; angle is positive
#otherwise left turning means angle is negative.
#assumes Arduino gives a continuous distance while it's going 1
#direction, otherwise returns 0. Angle gives updated angle.
def updateCurrCordinate(distance, angle):
    changeInDist = 0
    if distance != 0
        changeInDist = distance - prevDistance
    prevDistance = distance
    currAngle += angle
    currCord_x += changeInDist * cos(currAngle)
    currCord_y += changeInDist * sin(currAngle)

#Checks if the distances it detects is less than 50cm
#if so, adds the coordinates of the obstacles into the 2d list
def mapping(frontDist, leftDist, rightDist, backDist):
    if frontDist < 50
        obstacles.append((\
        round(currCord_x + frontDist * cos(currAngle), 1),\
        round(currCord_y + frontDist * sin(currAngle),1)))
    if leftDist < 50
        obstacles.append((\
        round(currCord_x + frontDist * cos(currAngle + 90),1),\
        round(currCord_y + frontDist * sin(currAngle + 90),1)))
    if rightDist < 50
        obstacles.append((\
        round(currCord_x + frontDist * cos(currAngle + 270),1),\
        round(currCord_y + frontDist * sin(currAngle + 270),1)))
    if backDist < 50
        obstacles.append((\
        round(currCord_x + frontDist * cos(currAngle + 180),1),\
        round(currCord_y + frontDist * sin(currAngle + 180),1)))

##if time permits; need to find a method to cluster the points together
