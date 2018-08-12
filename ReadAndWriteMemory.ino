

void readEEPROM(int startAdr, int maxLength, char* dest) {
  memset(dest, 0, sizeof(dest));
  EEPROM.begin(512);
  delay(10);
  for (int i = 0; i < maxLength; i++)
  {
    dest[i] = char(EEPROM.read(startAdr + i));
  }
  EEPROM.end();

}
//startAdr: offset (bytes), writeString: String to be written to EEPROM


void writeEEPROM(int startAdr, int laenge, char* writeString) {
  EEPROM.begin(512); //Max bytes of eeprom to use
  yield();
  //write to eeprom
  for (int i = 0; i < laenge; i++)
  {
    EEPROM.write(startAdr + i, writeString[i]);
  }
  EEPROM.commit();
  EEPROM.end();
}

void readSSID(char* dest) {
  readEEPROM(0, 32, dest);
}

void readSsidPass(char* dest) {
  readEEPROM(32, 32, dest);
}

void readUserName(char* dest) {
  readEEPROM(64, 32, dest);
}

void readUserPassword(char* dest) {
  readEEPROM(96, 32, dest);
}

void readDeviceDescription(char* dest) {
  readEEPROM(128, 32, dest);
}

void readDeviceID(char* dest){
  readEEPROM(160, 32, dest);
}

void writeSSID(char* writeString){
  writeEEPROM(0, 32, writeString);
}

void writeSsidPass(char* writeString){
  writeEEPROM(32, 32, writeString);
}

void writeUserName(char* writeString){
  writeEEPROM(64, 32, writeString);
}

void writeUserPassword(char* writeString){
  writeEEPROM(96, 32, writeString);
}

void writeDeviceDescription(char* writeString){
  writeEEPROM(128, 32, writeString);
}

void writeDeviceID(char* writeString){
  writeEEPROM(160, 32, writeString);
}

void cleanUpMemory() {
  EEPROM.begin(512);

  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
    Serial.println(".");
  }

  EEPROM.commit();
  EEPROM.end();
  endWifiSetup();

  configSetup();
  String readOut = "Done";
  server.send(200, "text/plain", readOut);
}

