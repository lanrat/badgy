static const int pinD0 = 2; // green
static const int pinD1 = 3; // white
static const int len = 35;
unsigned char dem;
unsigned char data[len];
unsigned long cardID;

void setup() {
  Serial.begin(9600);
  pinMode(pinD0, INPUT);
  pinMode(pinD1, INPUT);
  attachInterrupt(digitalPinToInterrupt(pinD0), ReadD0, FALLING); // Hardware interrupt - high to low pulse
  attachInterrupt(digitalPinToInterrupt(pinD1), ReadD1, FALLING); // Hardware interrupt - high to low pulse
  Serial.println("booted");
}
void ReadD0() {
  dem++;
  Serial.println("got 0");
}
void ReadD1() {
  data[dem] = 1;
  dem++;
  Serial.println("got 1");
}
void loop() {
  //Serial.println("L");
  //RFID card have 35 bit data
  //Please check number of bit data before using below code

  if (dem >= len) {
    for (unsigned char i = 0; i < len; i++) {
      cardID <<= 1;
      cardID |= data[i];
    }
    Serial.println(cardID, DEC);
    for (unsigned char i = 0; i < len; i++) {
      data[i] = 0;
    }
    dem = 0;
  }
  /* Code check number of bit data
    if(dem>0){
    Serial.println(dem);
    }
  */
}