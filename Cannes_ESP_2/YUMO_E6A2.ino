volatile int encoderLast = 0;
volatile long encoderValue = 0;
volatile int encoderAngle = 0;

int encoderPin1;
int encoderPin2;

void setupEncoder(int pin1, int pin2) {
  encoderPin1 = pin1;
  encoderPin2 = pin2;
  pinMode(encoderPin1, INPUT_PULLUP);
  pinMode(encoderPin2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderPin1), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPin2), updateEncoder, CHANGE);
}


void updateEncoder(){
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit

  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (encoderLast << 2) | encoded; //adding it to the previous encoded value

  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;

  encoderLast = encoded; //store this value for next time

  //Constrain to 1 tour - 360°
  if (encoderValue>799){ encoderValue = 0; }
  if (encoderValue<0){ encoderValue = 799; }
  encoderAngle = map(encoderValue, 0, 799, 0, 359);
}

long getEncoder() {
  return encoderValue;
}

int getEncoderAngle() {
  return encoderAngle;
}