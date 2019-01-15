#include <graphicsLCD.h>

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


#define lcd_RST_pin 14
#define lcd_CS_pin 12
#define lcd_DIN_pin 13
#define lcd_CLK_pin 15

#define I2C_CLK 0
#define I2C_DATA 2

graphicsLCD lcd(lcd_RST_pin,lcd_CS_pin,lcd_DIN_pin,lcd_CLK_pin);

Adafruit_BME280 bme; // I2C

// Replace with your network details
const char* ssid = "BIBLIOTECH";
const char* password = "smartcafe";

float h, t, p, pin, dp;
char temperatureString[6];
char dpString[6];
char humidityString[6];
char pressureString[7];
char pressureInchString[6];

String tempString;

// Web Server on port 80
WiFiServer server(80);


void setup() {

  lcd.begin();
  lcd.clear();

  // Initializing serial port for debugging purposes
  Serial.begin(115200);
  delay(10);
  Wire.begin(I2C_CLK, I2C_DATA);
  Wire.setClock(100000);
  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  lcd.line(0);
  lcd.print("Connecting to: ");
  lcd.line(1);
  char c2[20];
  strcpy(c2, ssid);
  lcd.print(c2);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    lcd.line(2);
    lcd.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  lcd.line(2);
  lcd.print("WiFi connected");
  
  // Starting the web server
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  
  // Printing the ESP IP address
  Serial.println(WiFi.localIP());
  Serial.println(F("BME280 test"));

  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

}

void getWeather() {
  
    h = bme.readHumidity();
    t = bme.readTemperature();
    
    p = bme.readPressure()/133.3F;
    pin = 0.02953*p;
    dtostrf(t, 4, 1, temperatureString);
    dtostrf(h, 4, 1, humidityString);
    dtostrf(p, 5, 1, pressureString);
    dtostrf(pin, 5, 1, pressureInchString);

    //print to display
    lcd.clear();
    lcd.line(0);
    lcd.print("Temperature: ");
    lcd.line(1);
    lcd.print(temperatureString);
    lcd.print(" C");
    lcd.line(2);
    lcd.print("Humidity: ");
    lcd.line(3);
    lcd.print(humidityString);
    lcd.print(" %");
    lcd.line(4);
    lcd.print("Pressure: ");
    lcd.line(5);
    lcd.print(pressureString);
    lcd.print(" mmHg");    
    delay(100);
}

void loop() { 
     // Listenning for new clients
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("New client");
    // bolean to locate when the http request ends
    boolean blank_line = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        if (c == '\n' && blank_line) {
            getWeather();
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            // your actual web page that displays temperature
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head><META HTTP-EQUIV=\"refresh\" CONTENT=\"15\"></head>");
            client.println("<body><h1>ESP8266 Weather Web Server</h1>");
            client.println("<table border=\"2\" width=\"456\" cellpadding=\"10\"><tbody><tr><td>");
            client.println("<h3>Temperature = ");
            client.println(temperatureString);
            client.println("&deg;C</h3><h3>Humidity = ");
            client.println(humidityString);
            client.println("%</h3><h3>Pressure = ");
            client.println(pressureString);
            client.println("hPa (");
            client.println(pressureInchString);
            client.println("Inch)</h3></td></tr></tbody></table></body></html>");  
            break;
        }
        if (c == '\n') {
          // when starts reading a new line
          blank_line = true;
        }
        else if (c != '\r') {
          // when finds a character on the current line
          blank_line = false;
        }
      }
    }  
    // closing the client connection
    delay(1);
    client.stop();
    Serial.println("Client disconnected.");
  }
}



