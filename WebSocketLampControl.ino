
const char* ws_host               = "192.168.1.24";
const int   ws_port               = 8080;
String ws_baseurl            = "/iot/iot/";
String deviceID            = "newDevice";
WebSocketsClient webSocket;
#define USE_SERIAL Serial;

void webSocketSetup(char device_id_private[32]) {
  if (strlen(device_id_private) == 0) {
    deviceID += micros();
  } else {
    deviceID = device_id_private;
  }
  String socketUrl = ws_baseurl + deviceID;
  Serial.println(socketUrl);
  // connect to websocket
  webSocket.begin(ws_host, ws_port, socketUrl);
  webSocket.onEvent(webSocketEvent);

}

void webSocketLoop() {
  webSocket.loop();
}

void registerDeviceWithSystem(String userName, String token, String deviceDescription) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["action"] = "registerNewDevice";
  root["userName"] = userName;
  root["deviceType"] = "Lamp";
  root["userToken"] = token;
  root["deviceDescription"] = deviceDescription;

  String packet;
  root.printTo(packet);
  webSocket.sendTXT(packet);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED:
      {
        //checks if ID of device was already set,
        //otherwise it will start registration process with websocket server
        if (strlen(device_id_private) == 0) {
          registerDeviceWithSystem(String(user_name_private), String(token_private), String(device_description_private));
        }
        Serial.printf("[WSc] Connected to url: %s\n",  payload);
      }
      break;
    case WStype_TEXT:
      {
        String text = (char*) payload;

        Serial.printf("[WSc] get text: %s\n", payload);

        DynamicJsonBuffer jsonBuffer;
        //Parsing JSON from POST Request into JSON object
        JsonObject& root = jsonBuffer.parseObject(payload);

        String action = root[String("action")];
        Serial.println(action);
        if (action == "lampStatus") {
          String from = root[String("from")];
          String to = root[String("to")];
          String handlerID = root[String("handlerID")];

          JsonObject& root = jsonBuffer.createObject();
          root["from"] = to;
          root["to"] = from;
          root["action"] = action;
          root["handlerID"] = handlerID;
          root["lampStatus"] = lampStatus();
          root["userName"] = user_name_private;
          root["userToken"] = token_private;
          String reply;
          root.printTo(reply);
          Serial.println(reply);
          webSocket.sendTXT(reply);

        } else if (action == "lampOn") {
          String from = root[String("from")];
          String to = root[String("to")];
          String handlerID = root[String("handlerID")];

          JsonObject& root = jsonBuffer.createObject();
          root["from"] = to;
          root["to"] = from;
          root["action"] = action;
          root["handlerID"] = handlerID;
          root["lampStatus"] = turnLampOn();
          root["userName"] = user_name_private;
          root["userToken"] = token_private;
          String reply;
          root.printTo(reply);
          webSocket.sendTXT(reply);

        } else if (action == "lampOff") {
          String from = root[String("from")];
          String to = root[String("to")];
          String handlerID = root[String("handlerID")];

          JsonObject& root = jsonBuffer.createObject();
          root["from"] = to;
          root["to"] = from;
          root["action"] = action;
          root["handlerID"] = handlerID;
          root["lampStatus"] = turnLampOff();
          root["userName"] = user_name_private;
          root["userToken"] = token_private;
          String reply;
          root.printTo(reply);
          webSocket.sendTXT(reply);
        } else if (action == "deviceRegistrationCompleted") {
          String deviceID = root[String("deviceID")];
          strcat(device_id_private, deviceID.c_str());
          writeDeviceID(device_id_private);
          //This will send a confirmation back to mobile device that registration was completed.
          server.send(200, "text/plain", "deviceRegistrationCompleted");
          //once the user details are confirm the system will save the data into EPROM memory.

          writeUserName(user_name_private);
          writeUserToken(token_private);
          writeDeviceDescription(device_description_private);
          ESP.restart();
        } else if (action == "registrationUnsuccessful") {
          //if the registration was unsucessful the device will notify mobile device.
          //the device will wipe the EPROM memory and reboot itself for another try.
          server.send(200, "text/plain", "registrationUnsuccessful");
          cleanUpMemory();
          ESP.restart();
        } else if (action == "databaseError") {
          server.send(200, "text/plain", "databaseError");
          cleanUpMemory();
          ESP.restart();
        }
        break;
      }
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);

      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
  }
}

