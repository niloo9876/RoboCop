import numpy as np
import cv2
import os
import pygame
import sys


count = 0

def detect_faces(f_cascade, colored_img, scaleFactor=1.5):
    global count
    # just making a copy of image passed, so that passed image is not changed
    img_copy = colored_img.copy()

    # convert the test image to gray image as opencv face detector expects gray images
    gray = cv2.cvtColor(img_copy, cv2.COLOR_BGR2GRAY)

    # let's detect multiscale (some images may be closer to camera than others) images
    faces = f_cascade.detectMultiScale(gray, scaleFactor=scaleFactor, minNeighbors=5);

    # go over list of faces and draw them as rectangles on original colored img
    for (x, y, w, h) in faces:
        cv2.rectangle(img_copy, (x, y), (x + w, y + h), (0, 255, 0), 2)
        count += 1
    return img_copy


face_cascade = cv2.CascadeClassifier(os.path.abspath('C:/Users/yanyi/Anaconda3/Library/etc/haarcascades/haarcascade_frontalface_default.xml'))

img = cv2.imread(os.path.abspath('C:/Users/yanyi/Documents/2018 Spring/UAS Notes and Information/IMP/p1 (1).jpg'))

face_img = detect_faces(face_cascade, img)

if count != 0:
    cv2.imwrite("face.png", face_img)

