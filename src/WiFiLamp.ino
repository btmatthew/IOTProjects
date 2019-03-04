

const int pin = 0;

byte relON[] = {0xA0, 0x01, 0x01, 0xA2};  
byte relOFF[] = {0xA0, 0x01, 0x00, 0xA1};

int value;
int maxValue;
String lampStat = "off";

String turnLampOn() {
  //pinMode(pin, OUTPUT);
  //digitalWrite(pin, LOW);
  Serial.write(relON, sizeof(relON));   
  lampStat = "on";
  return lampStatus();
}

String turnLampOff() {
  //pinMode(pin, OUTPUT);
  //digitalWrite(pin, HIGH);
  Serial.write(relOFF, sizeof(relOFF));  
  lampStat = "off";
  return lampStatus();
}

String lampStatus() {    
    return lampStat;
}

int normalization(int value) {
  if (value > maxValue) {
    maxValue = value;
  }
  if (maxValue <= 1 || value == 0) {
    return value;
  }
  int sign = 100;

  if (value < 0) {
    value = -value;
    sign = -100;
  }

  if (value > maxValue) {
    value = maxValue;
  }
  return log10(value + 1) / log10(maxValue) * sign;
}

