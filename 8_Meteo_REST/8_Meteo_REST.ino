
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ArduinoJson.h>

#define I2C_CLK 0
#define I2C_DATA 2

Adafruit_BME280 bme; // I2C

const char* ssid = "BIBLIOTECH";
const char* password = "smartcafe";

ESP8266WebServer server(80);

float h, t, p, pin, dp;
char temperatureString[6];
char dpString[6];
char humidityString[6];
char pressureString[7];
char pressureInchString[6];

String tempString;


void setup() {

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

  if (MDNS.begin("rest")) {
    Serial.println("MDNS responder started");
  }

  Wire.begin(I2C_CLK, I2C_DATA);
  Wire.setClock(100000);

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
 
//  server.on("/", [](){ digitalWrite(LED, 0); server.send(200, "text/plain", "hello from esp8266!");});

  server.on("/weather", getWeather);

  
  server.begin();
  Serial.println("HTTP server started");

}

void getWeather() {
  
    h = bme.readHumidity();
    t = bme.readTemperature();
    p = bme.readPressure()/133.3F;
    
    const size_t bufferSize = JSON_OBJECT_SIZE(3);
    DynamicJsonBuffer jsonBuffer(bufferSize);
    
    JsonObject& root = jsonBuffer.createObject();
    root["temperature"] = t;
    root["humidity"] = h;
    root["pressure"] = p;
    String temp="";
    root.printTo(temp);
    server.send(200, "text/json", temp);

}

void loop(){
  server.handleClient();
}
