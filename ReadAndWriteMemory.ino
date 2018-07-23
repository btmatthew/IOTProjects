#include <EEPROM.h>

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
