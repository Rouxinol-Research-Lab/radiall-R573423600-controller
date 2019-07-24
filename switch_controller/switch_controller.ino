#include <avr/eeprom.h>

/**************************************************************************************************/
/*                                                                                                */
/*                          Controller for the Radiall RF Switch.                                 */
/*                                                                                                */
/* The switch is controlled by sending DC pulses through 7 relays which in turn are controlled by */
/* an optical signal that changes the relay. There are 8 relays in the board but the last one is  */
/* not used. There is one-to-one relation between the pins 2 through 9 and the relay 1 through 8. */
/*                                                                                                */
/* This is the setting:  pin 2 - relay 8                                                          */
/*                       pin 3 - relay 7                                                          */
/*                       pin 4 - relay 6                                                          */
/*                       pin 5 - relay 5                                                          */
/*                       pin 6 - relay 4                                                          */
/*                       pin 7 - relay 3                                                          */
/*                       pin 8 - relay 2                                                          */
/*                       pin 9 - relay 1                                                          */
/*                                                                                                */
/* To activate the relay, one must send a LOW signal to its respective pin.                       */
/* The pin 11 is used to detect when the button is pressed through a pull up circuit. When that   */
/* happens the next relay will be activated for some time and them desactivated. Therefore the    */
/* relays are activated in decrescent order, starting at relay 7. When last relay activated was   */
/* relay 1, it is reset to relay 7. The last relay activated is also saved at the eeprom memory.  */
/* If the write/erase cycles pass over 90k, a warning will be sent through the serial channell.   */
/*                                                                                                */
/**************************************************************************************************/

// Memory address that saves the number of the last relay activated. 1 byte used.
#define ADDRESS_COMMAND 0

// Memory address that save how many times the command memory was written over. As the recommended
// number of write/erase cycles is 100k, it stores a 32bit integer.
#define ADDRESS_CYCLES ADDRESS_COMMAND + 1

// The is the variable that holds the number of write/erase cycles. if it is over 90k, an warning is
// sent through the serial channel.
uint32_t numberOfCycles = 0;

// relay pins
#define RELAY_1 9
#define RELAY_2 8
#define RELAY_3 7
#define RELAY_4 6
#define RELAY_5 5
#define RELAY_6 4
#define RELAY_7 3
#define RELAY_8 2

// button pin
#define ANTICLOCKWISE_PIN 11

// This variable holds the number of the last relay activated
int8_t presentCommand = 0;

// These variables are used to detect if the button was pressed.
// If the last button value was 1 and the actual value is 0, the button has
// just been pressed. 
uint8_t buttonValueAntiClockwise = 1;
uint8_t previousButtonValueAntiClockwise = 1;


// This variable holds how much the relay should be activate. This number should be calibrated.
uint8_t timeDelay = 50;

// Here we set all relay pins as output and and the relay as off.
// and then turn on serial communication and send a code to say it is ready ('o').
// We also read the last relay activated and the number of write/erase cycles.
void setup() {
  pinMode(RELAY_1,OUTPUT);
  pinMode(RELAY_2,OUTPUT);
  pinMode(RELAY_3,OUTPUT);
  pinMode(RELAY_4,OUTPUT);
  pinMode(RELAY_5,OUTPUT);
  pinMode(RELAY_6,OUTPUT);
  pinMode(RELAY_7,OUTPUT);
  pinMode(RELAY_8,OUTPUT);

  pinMode(ANTICLOCKWISE_PIN,INPUT);

  while (!eeprom_is_ready());
  
  presentCommand = eeprom_read_byte((uint8_t*)ADDRESS_COMMAND);
  numberOfCycles = eeprom_read_dword((uint32_t*)ADDRESS_CYCLES);
  
  // reset all relay
  click(-1);

  Serial.begin(9600);
  Serial.print("o ");
  Serial.println(presentCommand+1);
  Serial.print("# of EEPROM write/erase cycles: ");
  Serial.println(numberOfCycles);
  if(numberOfCycles>90000) {
    Serial.println("ALERT: The number of write/erase cycles at the EEPROM is over 90k.\n The max number allowed is 100k but it is recommended to change the memory addresses nonetheless.\n Update ADDRESS_COMMAND to the its actual value + 8. Remember that the memory size is limited to 512 bytes.");
  }
}

// This function in turn off all relay except the relay k
void click(int8_t relay_k) {
  for(int8_t relay = 2; relay <= 9; relay++) {
      digitalWrite(relay,HIGH);
  }
  if(relay_k >= 2 and relay_k <= 9)
    digitalWrite(relay_k,LOW);
}

// activate for a brief time just one relay
void change(int8_t command,uint8_t t) {
  
      // it is 9-command because the relay k is connected to pin 9-k.
      // it activate the right relay for t milliseconds.
      digitalWrite(9-command,LOW);
      delay(t);
      digitalWrite(9-command,HIGH);

      //Save the last relay number that has just been activated and update the number of cycles
      eeprom_write_byte((uint8_t*)ADDRESS_COMMAND,(uint8_t)presentCommand);
      numberOfCycles++;
      eeprom_write_dword((uint32_t*)ADDRESS_CYCLES,numberOfCycles);
}

// this function detects a command and activates the relay
void loop() {
  if(Serial.available()) {

    // we read the char sent to us, and subtract '0' which is 48 in integer.
    char command = Serial.read();
    // must be a valid relay or 0 which is the command to turn off all relay
    if(isDigit(command)) {
      presentCommand = (command-'0'-1)&7;

      change(presentCommand,timeDelay);
      Serial.print("serial: ");
      Serial.println((int)presentCommand+1);
    } else if (command != '\n') Serial.print('i');
  }

  // detect if button is pressed
  previousButtonValueAntiClockwise = buttonValueAntiClockwise;
  buttonValueAntiClockwise = digitalRead(ANTICLOCKWISE_PIN);
  if(buttonValueAntiClockwise == previousButtonValueAntiClockwise-1) {
    if(--presentCommand<0) presentCommand = 6;
    change(presentCommand,timeDelay);
    Serial.print("button: ");
    Serial.println((int)presentCommand+1);
  }
}
