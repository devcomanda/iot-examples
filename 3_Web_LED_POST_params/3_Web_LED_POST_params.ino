#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "BIBLIOTECH";
const char* password = "smartcafe";

ESP8266WebServer server(80);

const int LED = 4;

void handleLED() {
  if(server.hasArg("state")) {
    if(server.arg("state") == "on"){
      digitalWrite(LED,0);
      server.send(200, "text/plain", "200: LED turned on");
      return;
    }
    if(server.arg("state") == "off"){
      digitalWrite(LED,1);
      server.send(200, "text/plain", "200: LED turned off");
      return;
    }
  } else {
    server.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    return;
  }
}

void setup(){
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 1);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Trying to connect");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("myled")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", [](){ server.send(200, "text/plain", "hello from esp8266!"); });

  server.on("/led", HTTP_POST, handleLED);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(){
  server.handleClient();
}
