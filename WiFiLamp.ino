

const int pin = 13;

const int pResistor = 36;
int value;
int maxValue;


void setupLamp()
{
  pinMode(pResistor, INPUT);
}

String turnLampOn() {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delay(2000);
  //release
  pinMode(pin, INPUT);

  value = normalization(analogRead(pResistor));
  String readOut = "{";
  readOut += "\"sensorValue\":";
  readOut += value;
  readOut += "}";
  return readOut;
}

String turnLampOff() {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delay(2000);
  //release
  pinMode(pin, INPUT);

  value = normalization(analogRead(pResistor));
  String readOut = "{";
  readOut += "\"sensorValue\":";
  readOut += value;
  readOut += "}";
  return readOut;
}

String lampStatus() {
    return ""+normalization(analogRead(pResistor));
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

