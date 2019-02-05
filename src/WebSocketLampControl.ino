String ws_baseurl            = "/iot/iot/";
String deviceID            = "newDevice";


//#define USE_SERIAL Serial;

void webSocketSetup(char device_id_private[32], char server_address_number[32],char server_port_number[32]) {
  if (strlen(device_id_private) == 0) {
    deviceID += micros();
  } else {
    deviceID = device_id_private;
  }
  String socketUrl = ws_baseurl + deviceID;

  // connect to websocket
  webSocket.setReconnectInterval(1000);
  webSocket.begin(server_address_number, atoi(server_port_number), socketUrl);
  
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
        if (action == "lampstatus") {
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

        }else if(action == "removedevice"){
          String from = root[String("from")];
          String to = root[String("to")];
          String handlerID = root[String("handlerID")];

          JsonObject& root = jsonBuffer.createObject();
          root["from"] = to;
          root["to"] = from;
          root["action"] = "deviceremoved";
          root["handlerID"] = handlerID;
          root["userName"] = user_name_private;
          root["userToken"] = token_private;
          String reply;
          root.printTo(reply);
          Serial.println(reply);
          webSocket.sendTXT(reply);
          delay(3000);
          webSocket.disconnect();
          delay(3000);
          cleanUpMemory();
          delay(3000);
          ESP.restart();
          } else if (action == "lampon") {
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

        } else if (action == "lampoff") {
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
        }else if (action == "updatedevicedescription") {
          String from = root[String("from")];
          String to = root[String("to")];
          String handlerID = root[String("handlerID")];
          String newDeviceName = root[String("deviceDescription")];
          Serial.println(newDeviceName);
          //reseting array memory
          memset(device_description_private, 0, sizeof(device_description_private));
          strcat(device_description_private, newDeviceName.c_str());
          Serial.println(device_description_private);
          writeDeviceDescription(device_description_private);          
          JsonObject& root = jsonBuffer.createObject();
          root["from"] = to;
          root["to"] = from;
          root["action"] = "devicedescriptionupdated";
          root["handlerID"] = handlerID;
          root["userName"] = user_name_private;
          root["userToken"] = token_private;
          root["deviceDescription"] = device_description_private;
          String reply;
          root.printTo(reply);
          webSocket.sendTXT(reply);
        } else if (action == "deviceRegistrationCompleted") {
          String deviceID = root[String("deviceID")];
          strcat(device_id_private, deviceID.c_str());
          writeDeviceID(device_id_private);
          //This will send a confirmation back to mobile device that registration was completed.
          JsonObject& root = jsonBuffer.createObject();
          root["action"] = "deviceRegistrationCompleted";
          String reply;
          root.printTo(reply);
          server.send(200, "text/plain", reply);
          delay(3000); 
          //once the user details are confirm the system will save the data into EPROM memory.

          writeUserName(user_name_private);
          writeUserToken(token_private);
          writeDeviceDescription(device_description_private);
          writeServerAddress(server_address_number);
          writeServerPort(server_port_number);
          ESP.restart();
        } else if (action == "registrationUnsuccessful") {
          //if the registration was unsucessful the device will notify mobile device.
          //the device will wipe the EPROM memory and reboot itself for another try.
          JsonObject& root = jsonBuffer.createObject();
          root["action"] = "registrationUnsuccessful";
          String reply;
          root.printTo(reply);
          server.send(200, "text/plain", reply);
          delay(3000); 
          cleanUpMemory();
          ESP.restart();
        } else if (action == "databaseError") {

          JsonObject& root = jsonBuffer.createObject();
          root["action"] = "databaseError";
          String reply;
          root.printTo(reply);
          
          server.send(200, "text/plain", reply);
          delay(3000); 
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

