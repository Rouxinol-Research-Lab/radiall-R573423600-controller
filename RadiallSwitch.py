import serial
import time

# A module with classes to control the arduino with python

#A specific arduino error
# this will be called in the following cases:
# 1. the command sent to the arduino is invalid.
# 2. the serial connection with arduino is inexistent.
class ArduinoError(RuntimeError):
    pass

# Main class that controls the arduino.
# it has three important methods. the constructor which open a serial connection with the arduino,
# the click method which opens the relay k or close them all with command 0 and, finally, the close method
# which closes the serial connection.
class Switch():

    # Port in windows is usually "COM#" where # is a number.
    def __init__(self,port,baudrate=9600,timeout=0,waittime=0.1,attempts=100):
        self._waittime = waittime
        self._attempts = attempts

        #open connection
        self._stdout = serial.Serial(port,baudrate,timeout=timeout)

        # Tries to connect with arduino. it will know is ready when the response 3 is received.
        # raise an exception if it takes too long (default: 10 seconds)
        c = b''
        i = 0
        while(c != b'3'):
            c = self._stdout.readline();
            time.sleep(self._waittime)
            i += 1
            if(i == self._attempts):
                raise serial.SerialTimeoutException("Arduino is not responding.")
    
    def click(self,k):
        
        # checks that there is an open channel, raise an exception if there is not one.
        if(self._stdout.isOpen()):

            # k should be int
            if(type(k) != int):
                raise TypeError("Wrong parameter type. It should be an integer between 0 and 8.")

            # k should be a valid relay between 1 and 8 or should be 0 which turn them all off.
            if (k<0 or k >8):
                raise ValueError("Parameter is a wrong relay number. There are only 8 relays, therefore the parameter should be between 1 and 8. 0 is also permitted, as it is a command to close all relays.")

            # send a char byte
            self._stdout.write(bytes([48+k]))

            # wait and check for response
            time.sleep(self._waittime)
            result = int.from_bytes(self._stdout.read(),byteorder='big')-48

            # this should not really happen except in the weirdest of cases. Even though it may seem impossible, do not remove it. You would be surprised
            # of the kind of bugs that exist in software development. Let's be safe. 
            if(result == 1):
                raise ArduinoError("Command invalid. Something went very wrong because this should have been raised as an error before sending something to the arduino. Maybe you are out-of-sync with the arduino?")
        else:
            raise ArduinoError("Serial port access to arduino is closed.")
        
    def close(self):
        self._stdout.close()
