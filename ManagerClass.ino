#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

ESP8266WebServer server(80);
const char WiFiAPPSK[] = "password";

char wifi_ssid_private[32];
char wifi_password_private[32];

char user_name_private[32];
char user_password_private[32];

char device_id_private[32];
char device_description_private[32];

boolean webSocketBoolean = false;

void setup() {
  Serial.begin(115200);
  readSSID(wifi_ssid_private);
  readSsidPass(wifi_password_private);
  readDeviceID(device_id_private);
  readUserName(user_name_private);
  readUserPassword(user_password_private);
  readDeviceDescription(device_description_private);
  configSetup();
}

void loop() {
  if (webSocketBoolean) {
    webSocketLoop();
  } else {
    server.handleClient(); //Handling of incoming requests
  }
}

void configSetup() {

  //This if statement checks if the ssid array read from memory contains any characters
  //if not it will start the initial setup config
  //otherwise it will start the normal operation mode
  if (strlen(wifi_ssid_private) == 0) {
    webSocketBoolean = false;
    setupWiFiForSoftAP();
    server.on("/wifiSetUP", configBody); //Associate the handler function to the path
    server.begin(); //Start the server
  } else {
    setupWifiForSTA();
    webSocketBoolean = true;    
    webSocketSetup(device_id_private);
  }
}

void configBody() { //Handler for the body path

  if (server.hasArg("plain") == false) { //Check if body received

    server.send(200, "text/plain", "Body not received");
    return;

  }
  Serial.println("pass received");
  //String message = "Body received:\n";
  String message = server.arg("plain");
  //message += "\n";
  Serial.println(message);
  server.send(200, "text/plain", "detailsReceived");
  //Defining JSONBuffer
  DynamicJsonBuffer jsonBuffer;
  //Parsing JSON from POST Request into JSON object
  JsonObject& root = jsonBuffer.parseObject(message);

  String ssid = root[String("ssid")];

  String pass = root[String("pass")];

  String userName = root[String("userName")];

  String password = root[String("userPassword")];

  String description = root[String("description")];

  strcat(wifi_ssid_private, ssid.c_str());
  strcat(wifi_password_private, pass.c_str());
  strcat(user_name_private, userName.c_str());
  strcat(user_password_private, password.c_str());
  strcat(device_description_private, description.c_str());

  writeSSID(wifi_ssid_private);

  writeSsidPass(wifi_password_private);

  writeUserName(user_name_private);

  writeUserPassword(user_password_private);

  writeDeviceDescription(device_description_private);

  configSetup();

}

void endWifiSetup() {
  server.close();
}


