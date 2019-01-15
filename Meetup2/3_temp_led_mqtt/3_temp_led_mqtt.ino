#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
//MQTT client library
#include <PubSubClient.h>
//WS2812 LED control library
#include <Adafruit_NeoPixel.h>

#define PIN D4
#define NUM_LEDS 4
uint8_t curRed=0, curGreen=0, curBlue=0, curWait=250, curProg=0,brightness=255;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// Replace with your network details
const char* ssid = "DATA-HUB";
const char* password = "Datahubnetwork";

//set MQTT broker credentials
const char* mqttServer = "testmqtt.pp.ua"; 
const int mqttPort = 1883; 
const char* mqttUser = "testuser";
const char* mqttPassword = "password";

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
    if (client.connect(clientId.c_str(),mqttUser,mqttPassword)) {
//    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("room/light/brightness",1);
      client.subscribe("room/light/program",1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print(payload[i]);
  }
  Serial.println();

  if (strcmp(topic,"room/light/brightness") == 0) {
     brightness = payload[0];
     strip.setBrightness(brightness);
     strip.show();
     Serial.print("Brightness changed to ");
     Serial.println(brightness);
  }

  if (strcmp(topic,"room/light/program") == 0) {
     curProg = payload[0];
     Serial.print("LED program changed to ");
     Serial.println(curProg);
  }

}

void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);
  // Setup LED strip
  strip.setBrightness(brightness);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  setup_wifi();
  DS18B20.begin();
  client.setServer(mqttServer, mqttPort);
  //add callback
  client.setCallback(callback);
}

void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(100);
  getTemperature();
  Serial.println(temperatureCString);
  client.publish("room/temperature", temperatureCString);

  
  Serial.print("Current program: ");
  Serial.println(curProg);
  Serial.print("Current brightness: ");
  Serial.println(brightness);
  switch (curProg) {
      case 0:
        colorWipe(strip.Color(0,0,0),0);
        break;
      case 1:
        for (uint8_t i=1;i<8;i++) {
          colorWipe(strip.Color((i&1)*255, ((i&2)>>1)*255, ((i&4)>>2)*255), curWait);
          delay(curWait);
          colorWipe(strip.Color(0,0,0), curWait);
          delay(curWait);
        }
        break;
      case 2:
        for (uint8_t i=1;i<8;i++) {
          runningDot(strip.Color((i&1)*255, ((i&2)>>1)*255, ((i&4)>>2)*255), curWait);
          delay(curWait);
        }
        break;
      case 3:
        rainbow(20);
        break;
      case 4:
        whiteOverRainbow(200,75,5);
        break;
      case 5:
        pulseWhite(5);
        pulseWhite(5);
        pulseWhite(5);
        break;
      case 6:
        rainbowFade2White(20,3,0);
        break;        
      case 7:
        rainbowFade2White(10,3,0);
        break;  
      case 8:
        rainbowFade2White(5,3,0);
        break;  
      case 9:
        rainbowFade2White(2,3,0);
        break;          
      default:
        // statements
        break;
  }
//  delay(5000);
  
}   

//code for different LED modes

byte neopix_gamma[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

//
void blinkNTimes(uint32_t color, uint8_t wait, uint8_t times) {
  for (uint16_t j=0; j<times; j++) {
      for(uint16_t i=0; i<strip.numPixels(); i++) {   
        strip.setPixelColor(i, color);
      }
      strip.show();
      delay(wait);
      for(uint16_t i=0; i<strip.numPixels(); i++) {   
        strip.setPixelColor(i, 0);
      }
      strip.show();
      delay(wait);
  }

  delay(1000);
}

//hard blink
void hardBlink(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if (random(2) == 0) {
      strip.setPixelColor(i, 0);  
    } 
    else {
      strip.setPixelColor(i, c);
    }
    
    strip.show();
    
  }
  delay(random(100,wait));
}

// Running dot
void runningDot(uint32_t c, uint8_t wait) {
   for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
    strip.setPixelColor(i, 0);
  }
}

// Blinking star and skirt
void blinkStarRandom(uint8_t wait) {
  for (uint16_t j=0; j<100; j++){
    strip.setPixelColor(4, strip.Color(0, 0, 0));
    for (uint16_t i=0; i<strip.numPixels()-1;i++) {
      strip.setPixelColor(i, strip.Color(random(0,255), random(0,255), random(0,255)));
      strip.show();   
    }
  
    delay(wait);
  
    strip.setPixelColor(4, strip.Color(random(0,255), random(0,255), random(0,255)));
    for (uint16_t i=0; i<strip.numPixels()-1;i++) {
      strip.setPixelColor(i, strip.Color(0,0,0));
      strip.show();  
    }
    delay(wait);
  }
  
  
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void pulseWhite(uint8_t wait) {
  for(int j = 0; j < 256 ; j++){
      for(uint16_t i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(neopix_gamma[j], neopix_gamma[j], neopix_gamma[j]));
        }
        delay(wait);
        strip.show();
      }

  for(int j = 255; j >= 0 ; j--){
      for(uint16_t i=0; i<strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(neopix_gamma[j], neopix_gamma[j], neopix_gamma[j]) );
        }
        delay(wait);
        strip.show();
      }
}


void rainbowFade2White(uint8_t wait, int rainbowLoops, int whiteLoops) {
  float fadeMax = 100.0;
  int fadeVal = 0;
  uint32_t wheelVal;
  int redVal, greenVal, blueVal;

  for(int k = 0 ; k < rainbowLoops ; k ++){
    for(int j=0; j<256; j++) { // 5 cycles of all colors on wheel

      for(int i=0; i< strip.numPixels(); i++) {

        wheelVal = Wheel(((i * 256 / strip.numPixels()) + j) & 255);

        redVal = red(wheelVal) * float(fadeVal/fadeMax);
        greenVal = green(wheelVal) * float(fadeVal/fadeMax);
        blueVal = blue(wheelVal) * float(fadeVal/fadeMax);

        strip.setPixelColor( i, strip.Color( redVal, greenVal, blueVal ) );

      }

      //First loop, fade in!
      if(k == 0 && fadeVal < fadeMax-1) {
          fadeVal++;
      }

      //Last loop, fade out!
      else if(k == rainbowLoops - 1 && j > 255 - fadeMax ){
          fadeVal--;
      }

        strip.show();
        delay(wait);
    }
  
  }
  delay(500);
  for(int k = 0 ; k < whiteLoops ; k ++){
    for(int j = 0; j < 256 ; j++){
        for(uint16_t i=0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, strip.Color(neopix_gamma[j],neopix_gamma[j],neopix_gamma[j] ) );
          }
          strip.show();
        }
        delay(2000);
    for(int j = 255; j >= 0 ; j--){
        for(uint16_t i=0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, strip.Color(neopix_gamma[j],neopix_gamma[j], neopix_gamma[j] ) );
          }
          strip.show();
        }
  }
  delay(500);
}

void whiteOverRainbow(uint8_t wait, uint8_t whiteSpeed, uint8_t whiteLength ) {
  
  if(whiteLength >= strip.numPixels()) whiteLength = strip.numPixels() - 1;
  int head = whiteLength - 1;
  int tail = 0;
  int loops = 5;
  int loopNum = 0;
  static unsigned long lastTime = 0;
  while(true){
    for(int j=0; j<256; j++) {
      for(uint16_t i=0; i<strip.numPixels(); i++) {
        if((i >= tail && i <= head) || (tail > head && i >= tail) || (tail > head && i <= head) ){
          strip.setPixelColor(i, strip.Color(0,0,0, 255 ) );
        }
        else{
          strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
        }
      }
      if(millis() - lastTime > whiteSpeed) {
        head++;
        tail++;
        if(head == strip.numPixels()){
          loopNum++;
        }
        lastTime = millis();
      }
      if(loopNum == loops) return;
      head%=strip.numPixels();
      tail%=strip.numPixels();
        strip.show();
        delay(wait);
    }
  }
  
}
void fullWhite() {
  
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0,0,0, 255 ) );
    }
      strip.show();
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256 * 5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3,0);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3,0);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0,0);
}

uint8_t red(uint32_t c) {
  return (c >> 16);
}
uint8_t green(uint32_t c) {
  return (c >> 8);
}
uint8_t blue(uint32_t c) {
  return (c);
}

