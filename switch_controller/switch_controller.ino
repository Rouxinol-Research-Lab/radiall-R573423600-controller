#include <avr/eeprom.h>

#define ADDRESS_COMMAND 0
#define ADDRESS_CYCLES ADDRESS_COMMAND + 1
uint32_t numberOfCycles = 0;

#define RELAY_1 9
#define RELAY_2 8
#define RELAY_3 7
#define RELAY_4 6
#define RELAY_5 5
#define RELAY_6 4
#define RELAY_7 3
#define RELAY_8 2
#define RELAY_ALL 0

#define ANTICLOCKWISE_PIN 11

int8_t presentCommand = 0;

uint8_t buttonValueAntiClockwise = 1;
uint8_t previousButtonValueAntiClockwise = 1;

uint8_t timeDelay = 50;

// Here we set all relay pins as output and and the relay as off.
// and then turn on serial communication and send a code to say it is ready ('o').
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
  Serial.print("# of EEPROM cycles: ");
  Serial.println(numberOfCycles);
  if(numberOfCycles>90000) {
    Serial.println("ALERT: The number of write/erase cycles at the EEPROM is over 90k.\n The max number allowed is 100k but it is recommended to change the arduino chip nonetheless.");
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

// activate for a brief time just on relay
void change(int8_t command,uint8_t t) {
      digitalWrite(9-command,LOW);
      delay(t);
      digitalWrite(9-command,HIGH);
      eeprom_write_byte((uint8_t*)ADDRESS_COMMAND,(uint8_t)presentCommand);
      numberOfCycles++;
      eeprom_write_dword((uint32_t*)ADDRESS_CYCLES,numberOfCycles);
}

// this function detects a command a do something.
// when it detects something at the serial channel it responds with a code
// 1 to 8 - it changed the relay #.
// a - set to anticlockwise.
// c - set to clockwise.
// i - the command was invalid.
void loop() {
  if(Serial.available()) {

    // we read the char sent to us, and subtract '0' which is 48 in integer.
    char command = Serial.read();
    // must be a valid relay or 0 which is the command to turn off all relay
    if(isDigit(command)) {
      presentCommand = (command-'0'-1)&7;

	//ten is subtract by the value of the command because the pins are invarted in relation to the relay. that is:
	// pin 9 - relay 1
	// pin 8 - relay 2
	// ....
	// pin 3 - relay 7
	// pin 2 - relay 8
      change(presentCommand,timeDelay);
      Serial.print("serial: ");
      Serial.println((int)presentCommand+1);
    } else if (command != '\n') Serial.print('i');
  }

  previousButtonValueAntiClockwise = buttonValueAntiClockwise;
  buttonValueAntiClockwise = digitalRead(ANTICLOCKWISE_PIN);
  if(buttonValueAntiClockwise == previousButtonValueAntiClockwise-1) {
    if(--presentCommand<0) presentCommand = 6;
    change(presentCommand,timeDelay);
    Serial.print("button: ");
    Serial.println((int)presentCommand+1);
  }
}
