
String lampStat1 = "off";
String lampStat2 = "off";
String lampStat3 = "off";

String turnLampOn(int pin)
{
  switch (pin)
  {
  case 0:
  {
    lampStat1 = "on";
    lampStat2 = "on";
    lampStat3 = "on";
      pinMode(5, OUTPUT);
      digitalWrite(5, LOW);
      pinMode(4, OUTPUT);
      digitalWrite(4, LOW);
      pinMode(0, OUTPUT);
      digitalWrite(0, LOW);
    String allLamps = lampStat1 + "," + lampStat2 + "," + lampStat3;
    return allLamps;
    break;
  }
  case 1:
  {
    pinMode(5, OUTPUT);
    digitalWrite(5, LOW);
    lampStat1 = "on";
    return lampStat1;
    break;
  }
  case 2:
  {
    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);
    lampStat2 = "on";
    return lampStat2;
    break;
  }
  case 3:
  {
    pinMode(0, OUTPUT);
    digitalWrite(0, LOW);
    lampStat3 = "on";
    return lampStat3;
    break;
  }
  }
  return lampStat1;
}

String turnLampOff(int pin)
{
  switch (pin)
  {
  case 0:
  {
    lampStat1 = "off";
    lampStat2 = "off";
    lampStat3 = "off";
      pinMode(5, OUTPUT);
      digitalWrite(5, HIGH);
      pinMode(4, OUTPUT);
      digitalWrite(4, HIGH);
      pinMode(0, OUTPUT);
      digitalWrite(0, HIGH);
    String allLamps = lampStat1 + "," + lampStat2 + "," + lampStat3;
    return allLamps;
    break;
  }
  case 1:
  {
      pinMode(5, OUTPUT);
      digitalWrite(5, HIGH);
    lampStat1 = "off";
    return lampStat1;
    break;
  }
  case 2:
  {
      pinMode(4, OUTPUT);
      digitalWrite(4, HIGH);
    lampStat2 = "off";
    return lampStat2;
    break;
  }
  case 3:
  {
      pinMode(0, OUTPUT);
      digitalWrite(0, HIGH);
    lampStat3 = "off";
    return lampStat3;
    break;
  }
  }
  return lampStat1;
}

String lampStatus(){
      String allLamps = lampStat1 + "," + lampStat2 + "," + lampStat3;
    return allLamps;
}



