
const char* ws_host               = "192.168.1.24";
const int   ws_port               = 8080;
String ws_baseurl            = "/iot/iot/";
String deviceID            = "newDevice";
WebSocketsClient webSocket;
#define USE_SERIAL Serial;

void webSocketSetup(char device_id_private[32]) {
  if (strlen(device_id_private) == 0) {
    Serial.println("id is 0");
    deviceID += micros();
  } else {
    Serial.println("id is > 0");
    deviceID = device_id_private;
  }

  String socketUrl = ws_baseurl + deviceID;
  // connect to websocket
  webSocket.begin(ws_host, ws_port, socketUrl);

  //TODO check if this affects the code
  webSocket.setExtraHeaders(); // remove "Origin: file://" header because it breaks the connection with Spring's default websocket config
  webSocket.onEvent(webSocketEvent);
}

void webSocketLoop() {
  webSocket.loop();
}

void registerDeviceWithSystem(String userEmail, String password, String deviceDescription) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["action"] = "registerNewDevice";
  root["userEmail"] = userEmail;
  root["deviceType"] = "Lamp";
  root["password"] = password;
  root["deviceDescription"] = deviceDescription;

  String packet;
  root.printTo(packet);
  webSocket.sendTXT(packet);
}

void registerDeviceStatusWithSystem() {
  Serial.println("device is trying to register new session");
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
          registerDeviceWithSystem(String(user_email_private), String(user_password_private), String(device_description_private));
        } else {
          //todo complete this part of the system
          registerDeviceStatusWithSystem();
        }
        Serial.printf("[WSc] Connected to url: %s\n",  payload);
      }
      break;
    case WStype_TEXT:
      {
        // #####################
        // handle SockJs+STOMP protocol
        // #####################

        String text = (char*) payload;

        Serial.printf("[WSc] get text: %s\n", payload);


        DynamicJsonBuffer jsonBuffer;
        //Parsing JSON from POST Request into JSON object
        JsonObject& root = jsonBuffer.parseObject(payload);

        String action = root[String("action")];
        if (action == "status") {
          String from = root[String("from")];
          String to = root[String("to")];
          String handlerID = root[String("handlerID")];
          Serial.println(from);
          Serial.println(to);
          Serial.println(action);
          Serial.println(handlerID);
          JsonObject& root = jsonBuffer.createObject();
          root["from"] = to;
          root["to"] = from;
          root["action"] = action;
          root["handlerID"] = handlerID;
          String reply;
          root.printTo(reply);
          webSocket.sendTXT(reply);
        }else if (action =="deviceRegistrationCompleted") {
          String deviceID = root[String("deviceID")];
          strcat(device_id_private, deviceID.c_str());
          writeDeviceID(device_id_private);
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

