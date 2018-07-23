#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

ESP8266WebServer server(80);
const char WiFiAPPSK[] = "password";

char wifi_ssid_private[32];
char wifi_password_private[32];

void setup() {

  Serial.begin(115200);
  configSetup();
}

void configSetup() {
  readEEPROM(0, 32, wifi_ssid_private);
  readEEPROM(32, 32, wifi_password_private);

  if (strlen(wifi_ssid_private) == 0) {
    setupWiFiForSoftAP();
    server.on("/wifiSetUP", handleBody); //Associate the handler function to the path
    server.begin(); //Start the server
  } else {
    setupWifiForSTA();
    //todo test all of the functionality
    server.on("/status", handleStatus); //Associate the handler function to the path
    server.on("/on", turnLampOn); //Associate the handler function to the path
    server.on("/off", turnLampOff); //Associate the handler function to the path
    server.begin(); //Start the server
  }
}

void loop() {

  server.handleClient(); //Handling of incoming requests

}



void handleBody() { //Handler for the body path

  if (server.hasArg("plain") == false) { //Check if body received

    server.send(200, "text/plain", "Body not received");
    return;

  }

  //String message = "Body received:\n";
  String message = server.arg("plain");
  //message += "\n";

  server.send(200, "text/plain", "detailsReceived");
  //Defining JSONBuffer
  DynamicJsonBuffer jsonBuffer;
  //Parsing JSON from POST Request into JSON object
  JsonObject& root = jsonBuffer.parseObject(message);

  String ssid = root[String("ssid")];

  String pass = root[String("pass")];

  strcat(wifi_ssid_private, ssid.c_str());
  strcat(wifi_password_private, pass.c_str());

  writeEEPROM(0, 32, wifi_ssid_private);
  writeEEPROM(32, 32, wifi_password_private);

  //todo implement wifi setup closing

  Serial.println(wifi_ssid_private);

  Serial.println(wifi_password_private);

  endWiFiSetup();
  configSetup();
  
}

void endWiFiSetup() {
  server.close();
}

void setupWifiForSTA() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid_private, wifi_password_private);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}


void setupWiFiForSoftAP() {
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "ESPLamp " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i = 0; i < AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}


