#include <ArduinoJson.h>
#include <DHTesp.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

ESP8266WebServer server(80);
const char WiFiAPPSK[] = "password1";

char wifi_ssid_private[32];
char wifi_password_private[32];

char user_name_private[32];
char token_private[32];

char device_id_private[32];
char device_description_private[32];

boolean webSocketBoolean = false;
WebSocketsClient webSocket;
int count = 0;

DHTesp dht;

void setup() {
    Serial.begin(115200);
    readSSID(wifi_ssid_private);
    readSsidPass(wifi_password_private);
    readDeviceID(device_id_private);
    readUserName(user_name_private);
    readUserToken(token_private);
    readDeviceDescription(device_description_private);
    configSetup();
}

void loop() {
    if (webSocketBoolean) {
        count++;
        webSocketLoop();
        if (count == 1000000) {
            webSocket.sendPing();
            count = 0;
        }
    } else {
        server.handleClient();  //Handling of incoming requests
    }
}

void configSetup() {
    //This if statement checks if the ssid array read from memory contains any characters
    //if not it will start the initial setup config
    //otherwise it will start the normal operation mode
    Serial.print("lenght of wifipassword is ");
    Serial.println(strlen(wifi_ssid_private));
    Serial.print("ssid is ");
    Serial.println(wifi_ssid_private);
    if (strlen(wifi_ssid_private) == 0) {
        webSocketBoolean = false;
        setupWiFiForSoftAP();
        server.on("/wifiSetUP", configBody);  //Associate the handler function to the path
        server.begin();                       //Start the server
    } else {
        dht.setup(D4, DHTesp::DHT11);
        setupWifiForSTA();
        webSocketBoolean = true;
        webSocketSetup(device_id_private);
    }
}

void configBody() {  //Handler for the body path

    if (server.hasArg("plain") == false) {  //Check if body received
        server.send(200, "text/plain", "Body not received");
        return;
    }
    String message = server.arg("plain");

    //server.send(200, "text/plain", "Messages received");
    //message += "\n";
    Serial.println(message);
    //Defining JSONBuffer
    DynamicJsonBuffer jsonBuffer;
    //Parsing JSON from POST Request into JSON object
    JsonObject& root = jsonBuffer.parseObject(message);

    String ssid = root[String("ssid")];
    String pass = root[String("pass")];
    String userName = root[String("userName")];
    String token = root[String("userToken")];
    String description = root[String("description")];

    //this will take the data received from JSON and store it into char array
    strcat(wifi_ssid_private, ssid.c_str());
    strcat(wifi_password_private, pass.c_str());
    strcat(user_name_private, userName.c_str());
    strcat(token_private, token.c_str());
    strcat(device_description_private, description.c_str());

    //system will try to connect to wifi at the same time as keeping the connection with mobile device

    boolean ifWifi = setupWifiForRegistration(wifi_ssid_private, wifi_password_private);

    if (ifWifi) {
        //once the wifi details are confirmed the details will be saved in EMPROM memory
        writeSSID(wifi_ssid_private);
        writeSsidPass(wifi_password_private);
        webSocketBoolean = true;
        webSocketSetup(device_id_private);

    } else {
        JsonObject& root = jsonBuffer.createObject();
        root["action"] = "wifiError";
        String reply;
        root.printTo(reply);
        server.send(200, "text/plain", reply);
        delay(3000);
        webSocketBoolean = false;
        ESP.restart();
    }
}

void endWifiSetup() {
    server.close();
}
