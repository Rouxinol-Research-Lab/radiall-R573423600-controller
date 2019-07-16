import serial
import time

class ArduinoError(RuntimeError):
    pass

class Switch():
    def __init__(self,port,baudrate=9600,timeout=0,waittime=0.1,attempts=100):
        self._waittime = waittime
        self._attempts = attempts
        self._stdout = serial.Serial(port,baudrate,timeout=timeout)
        c = b''
        i = 0
        while(c != b'3'):
            c = self._stdout.readline();
            time.sleep(self._waittime)
            i += 1
            if(i == self._attempts):
                raise serial.SerialTimeoutException("Arduino is not responding.")
    
    def click(self,k):
        if(self._stdout.isOpen()):
            if(type(k) != int):
                raise TypeError("Wrong parameter type. It should be an integer between 0 and 8.")
            if (k<0 or k >8):
                raise ValueError("Parameter is a wrong relay number. There are only 8 relays, therefore the parameter should be between 1 and 8. 0 is also permitted, as it is a command to close all relays.")
        
            self._stdout.write(bytes([48+k]))
            time.sleep(self._waittime)
            result = int.from_bytes(self._stdout.read(),byteorder='big')-48
            if(result == 1):
                raise ArduinoError("Command invalid. Something went very wrong because this should have been raised as an error before sending something to the arduino. Maybe you are out-of-sync with the arduino?")
        else:
            raise ArduinoError("Serial port access to arduino is closed.")
        
    def close(self):
        self._stdout.close()
