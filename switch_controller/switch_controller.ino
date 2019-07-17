#define RELAY_1 9
#define RELAY_2 8
#define RELAY_3 7
#define RELAY_4 6
#define RELAY_5 5
#define RELAY_6 4
#define RELAY_7 3
#define RELAY_8 2
#define RELAY_ALL 0


char presentCommand = 0;

char buttonValueClockwise = 1;
char previousButtonValueClockwise = 1;

char buttonValueAntiClockwise = 1;
char previousButtonValueAntiClockwise = 1;

int timeDelay = 100;

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

  pinMode(10,INPUT);
  pinMode(11,INPUT);
  click(-1);

  Serial.begin(9600);
  Serial.print('o');
  
}

// This function in turn off all relay except the relay k
void click(int relay_k) {
  for(int relay = 2; relay <= 9; relay++) {
      digitalWrite(relay,HIGH);
  }
  if(relay_k >= 2 and relay_k <= 9)
    digitalWrite(relay_k,LOW);
}

void change(char command,int t) {
      digitalWrite(9-command,LOW);
      delay(t);
      digitalWrite(9-command,HIGH);
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
      change(9-presentCommand,timeDelay);
      Serial.print((int)presentCommand+1);
    } else if (command != '\n') Serial.print('i');
  }

  previousButtonValueClockwise = buttonValueClockwise;
  buttonValueClockwise = digitalRead(10);
  if(buttonValueClockwise == previousButtonValueClockwise-1) {
    if(++presentCommand>5)presentCommand = 0;
    change(presentCommand,timeDelay);
    Serial.print((int)presentCommand+1);
  }

  previousButtonValueAntiClockwise = buttonValueAntiClockwise;
  buttonValueAntiClockwise = digitalRead(11);
  if(buttonValueAntiClockwise == previousButtonValueAntiClockwise-1) {
    if(--presentCommand<0) presentCommand = 5;
    change(presentCommand,timeDelay);
    Serial.print((int)presentCommand+1);
  }
}
