#define RELAY_1 9
#define RELAY_2 8
#define RELAY_3 7
#define RELAY_4 6
#define RELAY_5 5
#define RELAY_6 4
#define RELAY_7 3
#define RELAY_8 2
#define RELAY_ALL 0

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

void click(int relay_k) {
  for(int relay = 2; relay <= 9; relay++) {
    if(relay == relay_k)
      digitalWrite(relay,LOW);
    else
      digitalWrite(relay,HIGH);
  }
}

int presentCommand = 0;

void loop() {
  if(Serial.available()) {
    char command = Serial.read() - '0';
    if(command >= 0 && command <= 8) {
      if(command != presentCommand) {
        presentCommand = command;
        click(10-presentCommand);
        Serial.print(0);
      } else Serial.print(2);
    } else if (command != '\n'-'0') Serial.print(1);
  }
}
