import serial
import threading
import queue
from time import sleep

def printMessage():
    while 1:
        print("Main is htere")
        sleep(2)

def printQueue():
    while True:
        if not serial_message.empty():
            print(serial_message.get())
            serial_message.task_done()


def serialReadinng(ser, serial_message):
    while True:
        serial_message.put(ser.readline())

        # x = ser.read()  # read one byte
        # s = ser.read(10)  # read up to ten bytes (timeout)
        # line = ser.readline()  # read a '\n' terminated line


serial_message = queue.Queue()

ser = serial.Serial('COM5', 9600, timeout=2)

thread1 = threading.Thread(target=serialReadinng, args=(ser, serial_message ))
thread2 = threading.Thread(target=printMessage)
thread3 = threading.Thread(target=printQueue)

thread1.start()
thread2.start()
thread3.start()

thread1.join()
thread2.join()
thread3.join()