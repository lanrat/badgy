const char* card = "11111110100000010011010111011001000";
const int pinD0 = 4;
const int pinD1 = 5;

const int signal_delay = 40; //ms

void send0() {
  digitalWrite(pinD0, LOW);
  delayMicroseconds(signal_delay);
  digitalWrite(pinD0, HIGH);
}

void send1() {
  digitalWrite(pinD1, LOW);
  delayMicroseconds(signal_delay);
  digitalWrite(pinD1, HIGH);
}

void sendCard(const char* card) {
  if (card == NULL) {
    Serial.println("no card");
    return;
  }
  for (int i = 0; card[i] != 0; i++) {
    if (card[i] == '0') {
      send0();
      //Serial.print("0");
    }else if (card[i] == '1') {
      send1();
      //Serial.print("1");
    }else {
      Serial.println("invalid card char");
      return;
    }
    delay(2);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(pinD0, OUTPUT);
  pinMode(pinD1, OUTPUT);

  digitalWrite(pinD0, HIGH);
  digitalWrite(pinD1, HIGH);

  //delay(100); // just to be safe
  Serial.println("Ready...");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Sending...");
  Serial.println(card);
  sendCard(card);
  Serial.println();
  delay(1000);


}