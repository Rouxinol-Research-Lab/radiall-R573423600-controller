#define RELAY_1 9
#define RELAY_2 8
#define RELAY_3 7
#define RELAY_4 6
#define RELAY_5 5
#define RELAY_6 4
#define RELAY_7 3
#define RELAY_8 2
#define RELAY_ALL 0

// Here we set all relay pins as output and and the relay as off.
// and then turn on serial communication and send a code to say it is ready (3).
void setup() {
  pinMode(RELAY_1,OUTPUT);
  pinMode(RELAY_2,OUTPUT);
  pinMode(RELAY_3,OUTPUT);
  pinMode(RELAY_4,OUTPUT);
  pinMode(RELAY_5,OUTPUT);
  pinMode(RELAY_6,OUTPUT);
  pinMode(RELAY_7,OUTPUT);
  pinMode(RELAY_8,OUTPUT);

  digitalWrite(RELAY_1,HIGH);
  digitalWrite(RELAY_2,HIGH);
  digitalWrite(RELAY_3,HIGH);
  digitalWrite(RELAY_4,HIGH);
  digitalWrite(RELAY_5,HIGH);
  digitalWrite(RELAY_6,HIGH);
  digitalWrite(RELAY_7,HIGH);
  digitalWrite(RELAY_8,HIGH);

  Serial.begin(9600);
  Serial.print(3);
  
}

// This function in turn off all relay except the relay k
void click(int relay_k) {
  for(int relay = 2; relay <= 9; relay++) {
    if(relay == relay_k)
      digitalWrite(relay,LOW);
    else
      digitalWrite(relay,HIGH);
  }
}

int presentCommand = 0;


// this function detects a command a do something.
// when it detects something at the serial channel it responds with a code
// 0 - it changed the relay.
// 1 - the relay is already on; did nothing.
// 2 - the command was invalid.
void loop() {
  if(Serial.available()) {

    // we read the char sent to us, and subtract '0' which is 48 in integer.
    char command = Serial.read() - '0';

    // must be a valid relay or 0 which is the command to turn off all relay
    if(command >= 0 && command <= 8) {

      // it must be a new relay
      if(command != presentCommand) {
	
        presentCommand = command;

	//ten is subtract by the value of the command because the pins are invarted in relation to the relay. that is:
	// pin 9 - relay 1
	// pin 8 - relay 2
	// ....
	// pin 3 - relay 7
	// pin 2 - relay 8
        click(10-presentCommand);
        Serial.print(0);
      } else Serial.print(2);
    } else if (command != '\n'-'0') Serial.print(1);
  }
}
