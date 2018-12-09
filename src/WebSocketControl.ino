
const char *ws_host = "matthewbulat.com";
const int ws_port = 8080;
String ws_baseurl = "/iot/iot/";
String deviceID = "newDevice";

void webSocketSetup(char device_id_private[32]) {
    if (strlen(device_id_private) == 0) {
        deviceID += micros();
    } else {
        deviceID = device_id_private;
    }
    String socketUrl = ws_baseurl + deviceID;
    Serial.println(socketUrl);
    // connect to websocket
    webSocket.setReconnectInterval(1000);
    webSocket.begin(ws_host, ws_port, socketUrl);

    webSocket.onEvent(webSocketEvent);
}

void webSocketLoop() {
    webSocket.loop();
}

void registerDeviceWithSystem(String userName, String token, String deviceDescription) {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["action"] = "registerNewDevice";
    root["userName"] = userName;
    root["deviceType"] = "IrRemote";
    root["userToken"] = token;
    root["deviceDescription"] = deviceDescription;

    String packet;
    root.printTo(packet);
    webSocket.sendTXT(packet);
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED:
            //checks if ID of device was already set,
            //otherwise it will start registration process with websocket server
            if (strlen(device_id_private) == 0) {
                registerDeviceWithSystem(String(user_name_private), String(token_private), String(device_description_private));
            }
            Serial.printf("[WSc] Connected to url: %s\n", payload);
            break;
        case WStype_TEXT: {
            //String text = (char *)payload;

            Serial.printf("[WSc] get text: %s\n", payload);

            DynamicJsonBuffer jsonBuffer;
            //Parsing JSON from POST Request into JSON object
            JsonObject &root = jsonBuffer.parseObject(payload);

            String action = root[String("action")];

            if (action == "remoteaction") {
                remoteaction(root);
            } else if (action == "deviceStatus") {
                deviceStatus(root);
            } else if (action == "removedevice") {
                removedevice(root);
            } else if (action == "updatedevicedescription") {
                updatedevicedescription(root);
            } else if (action == "deviceRegistrationCompleted") {
                deviceRegistrationCompleted(root);

            } else if (action == "registrationUnsuccessful") {
                //if the registration was unsucessful the device will notify mobile device.
                //the device will wipe the EPROM memory and reboot itself for another try.
                registrationUnsuccessful();

            } else if (action == "databaseError") {
                databaseError();
            }
        } break;
        case WStype_BIN:
            Serial.printf("[WSc] get binary length: %u\n", length);
            hexdump(payload, length);

            // send data to server
            // webSocket.sendBIN(payload, length);
            break;
    }
}

void remoteaction(JsonObject &root) {
    DynamicJsonBuffer jsonBuffer;
    String from = root[String("from")];
    String to = root[String("to")];
    String handlerID = root[String("handlerID")];
    int fanOption = root[String("fanOption")];

    String actionCompleted = fanAction(fanOption);
    JsonObject &rootReply = jsonBuffer.createObject();
    rootReply["from"] = to;
    rootReply["to"] = from;
    rootReply["action"] = actionCompleted;
    rootReply["handlerID"] = handlerID;
    rootReply["userName"] = user_name_private;
    rootReply["userToken"] = token_private;
    if (fanOption >= 1 && fanOption <= 5) {
        rootReply["fanStatus"] = (fanStatus == 0) ? false : true;
        rootReply["fanSpeed"] = fanSpeed;
        rootReply["fanMode"] = fanMode;
        rootReply["rotation"] = (rotation == 0) ? false : true;
        rootReply["ion"] = (ion == 0) ? false : true;
    } else if (fanOption >= 6 && fanOption <= 10) {
        rootReply["tvStatus"] = (tvStatus == 0) ? false : true;
    }
    String replyMessage;
    rootReply.printTo(replyMessage);
    Serial.println(replyMessage);
    webSocket.sendTXT(replyMessage);
}

void deviceStatus(JsonObject &root) {
    DynamicJsonBuffer jsonBuffer;
    String action = root[String("action")];
    String from = root[String("from")];
    String to = root[String("to")];
    String handlerID = root[String("handlerID")];

    JsonObject &rootReply = jsonBuffer.createObject();
    rootReply["from"] = to;
    rootReply["to"] = from;
    rootReply["action"] = action;
    rootReply["handlerID"] = handlerID;
    rootReply["tvStatus"] = (tvStatus == 0) ? false : true;
    rootReply["fanStatus"] = (fanStatus == 0) ? false : true;
    rootReply["fanSpeed"] = fanSpeed;
    rootReply["fanMode"] = fanMode;
    rootReply["rotation"] = (rotation == 0) ? false : true;
    rootReply["ion"] = (ion == 0) ? false : true;
    rootReply["userName"] = user_name_private;
    rootReply["userToken"] = token_private;
    String replyMessage;
    rootReply.printTo(replyMessage);
    Serial.println(replyMessage);
    webSocket.sendTXT(replyMessage);
}

void removedevice(JsonObject &root) {
    DynamicJsonBuffer jsonBuffer;
    String from = root[String("from")];
    String to = root[String("to")];
    String handlerID = root[String("handlerID")];

    JsonObject &rootReply = jsonBuffer.createObject();
    rootReply["from"] = to;
    rootReply["to"] = from;
    rootReply["action"] = "deviceremoved";
    rootReply["handlerID"] = handlerID;
    rootReply["userName"] = user_name_private;
    rootReply["userToken"] = token_private;
    String reply;
    rootReply.printTo(reply);
    Serial.println(reply);
    webSocket.sendTXT(reply);
    delay(3000);
    webSocket.disconnect();
    delay(3000);
    cleanUpMemory();
    delay(3000);
    ESP.restart();
}

void updatedevicedescription(JsonObject &root) {
    DynamicJsonBuffer jsonBuffer;
    String from = root[String("from")];
    String to = root[String("to")];
    String handlerID = root[String("handlerID")];
    String newDeviceName = root[String("deviceDescription")];
    //reseting array memory
    memset(device_description_private, 0, sizeof(device_description_private));
    strcat(device_description_private, newDeviceName.c_str());
    Serial.println(device_description_private);
    writeDeviceDescription(device_description_private);
    JsonObject &rootReply = jsonBuffer.createObject();
    rootReply["from"] = to;
    rootReply["to"] = from;
    rootReply["action"] = "devicedescriptionupdated";
    rootReply["handlerID"] = handlerID;
    rootReply["userName"] = user_name_private;
    rootReply["userToken"] = token_private;
    rootReply["deviceDescription"] = device_description_private;
    String reply;
    rootReply.printTo(reply);
    webSocket.sendTXT(reply);
}

void deviceRegistrationCompleted(JsonObject &root) {
    DynamicJsonBuffer jsonBuffer;
    String deviceID = root[String("deviceID")];
    strcat(device_id_private, deviceID.c_str());
    writeDeviceID(device_id_private);
    //This will send a confirmation back to mobile device that registration was completed.
    JsonObject &rootReply = jsonBuffer.createObject();
    rootReply["action"] = "deviceRegistrationCompleted";
    String reply;
    rootReply.printTo(reply);
    server.send(200, "text/plain", reply);
    delay(3000);
    //once the user details are confirm the system will save the data into EPROM memory.

    writeUserName(user_name_private);
    writeUserToken(token_private);
    writeDeviceDescription(device_description_private);
    ESP.restart();
}

void registrationUnsuccessful() {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["action"] = "registrationUnsuccessful";
    String reply;
    root.printTo(reply);
    server.send(200, "text/plain", reply);
    delay(3000);
    cleanUpMemory();
    ESP.restart();
}

void databaseError() {
    DynamicJsonBuffer jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["action"] = "databaseError";
    String reply;
    root.printTo(reply);

    server.send(200, "text/plain", reply);
    delay(3000);
    cleanUpMemory();
    ESP.restart();
}