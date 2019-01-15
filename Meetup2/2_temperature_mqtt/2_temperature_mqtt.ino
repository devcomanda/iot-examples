#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//MQTT client library
#include <PubSubClient.h>

// Replace with your network details
const char* ssid = "DATA-HUB";
const char* password = "Datahubnetwork";

//set MQTT broker credentials
const char* mqttServer = "testmqtt.pp.ua"; 
const int mqttPort = 1883; 
//const char* mqttUser = "testuser";
//const char* mqttPassword = "password";

//and create client
WiFiClient espClient;
PubSubClient client(espClient);

#define ONE_WIRE_BUS D1

OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature DS18B20(&oneWire);
char temperatureCString[7];

// Web Server on port 80
WiFiServer server(80);

void setup_wifi() {
  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
}

void getTemperature() {
  float tempC;
  do {
    DS18B20.requestTemperatures(); 
    tempC = DS18B20.getTempCByIndex(0);
    dtostrf(tempC, 2, 2, temperatureCString);
    delay(100);
  } while (tempC == 85.0 || tempC == (-127.0));
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
//    if (client.connect(clientId.c_str(),mqttUser,mqttPassword)) {
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);
  setup_wifi();
  DS18B20.begin();
  client.setServer(mqttServer, mqttPort);
}

void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  getTemperature();
  Serial.println(temperatureCString);
  client.publish("room/temperature", temperatureCString);
  delay(5000);
  
}   
