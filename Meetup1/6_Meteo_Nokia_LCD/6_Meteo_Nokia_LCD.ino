#include <graphicsLCD.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

// define software SPI pins
#define lcd_RST_pin 14 //RESET
#define lcd_CS_pin 12 //CS
#define lcd_DIN_pin 13 //MOSI
#define lcd_CLK_pin 15 //SCLK

graphicsLCD lcd(lcd_RST_pin,lcd_CS_pin,lcd_DIN_pin,lcd_CLK_pin);

// Replace with your network details
const char* ssid = "BIBLIOTECH";
const char* password = "smartcafe";

String APIKEY = "265ddac5b8abc1f2df055b5b52c69585";                                 
String CityID = "703448";                                 //Kyiv, UA

WiFiClient client;
char servername[]="api.openweathermap.org";              // remote server we will connect to
String result;

int  counter = 60;

String weatherDescription ="";
String weatherLocation = "";
String Country;
float Temperature;
float Humidity;
float Pressure;

String tempString;

void setup() {
  
  lcd.begin();
  lcd.clear();

  // Connecting to WiFi network
  lcd.line(0);
  lcd.print("Connecting to: ");
  lcd.line(1);
  char c2[20];
  strcpy(c2, ssid);
  lcd.print(c2);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.line(2);
    lcd.print(".");
  }
  lcd.line(2);
  lcd.print("WiFi connected");
  
}

void getWeatherData()                                //client function to send/receive GET request data.
{
  if (client.connect(servername, 80))   
    {                                         //starts client connection, checks for connection
      client.println("GET /data/2.5/weather?id="+CityID+"&units=metric&APPID="+APIKEY);
      client.println("Host: api.openweathermap.org");
      client.println("User-Agent: ArduinoWiFi/1.1");
      client.println("Connection: close");
      client.println();
    } 
  else 
    {
      lcd.print("connection failed");        //error message if no client connect
    }

  while(client.connected() && !client.available()) 
  delay(1);                                          //waits for data
  while (client.connected() || client.available())    
       {                                             //connected or data available
         char c = client.read();                     //gets byte from ethernet buffer
         result = result+c;
       }

  client.stop();                                      //stop client
  result.replace('[', ' ');
  result.replace(']', ' ');
//  Serial.println(result);
  char jsonArray [result.length()+1];
  result.toCharArray(jsonArray,sizeof(jsonArray));
  jsonArray[result.length() + 1] = '\0';
  StaticJsonBuffer<1024> json_buf;
  JsonObject &root = json_buf.parseObject(jsonArray);
  
  if (!root.success())
    {
      lcd.print("parseObject() failed");
    }
  
  String location = root["name"];
  String country = root["sys"]["country"];
  float temperature = root["main"]["temp"];
  float humidity = root["main"]["humidity"];
  String weather = root["weather"]["main"];
  String description = root["weather"]["description"];
  float pressure = root["main"]["pressure"];
  weatherDescription = description;
  weatherLocation = location;
  Country = country;
  Temperature = temperature;
  Humidity = humidity;
  Pressure = pressure;

}

void displayWeather(String location,String description)
{
  char charBuf[50];
  lcd.clear();
  lcd.line(0);
  location.toCharArray(charBuf, 20);
  lcd.print(charBuf);
  lcd.print(", ");
  Country.toCharArray(charBuf, 20);
  lcd.print(charBuf);
  lcd.line(1);
  description.toCharArray(charBuf, 50);
  lcd.print(charBuf);
}


void displayConditions(float Temperature,float Humidity, float Pressure)
{
   char tempStr[6];
   char humStr[6];
   char presStr[7];
   lcd.clear();                            //Printing Temperature
   lcd.line(0);
   lcd.print("T:");
   dtostrf(Temperature, 4, 1, tempStr);
   lcd.print(tempStr);
   lcd.print("C "); 
   lcd.line(2);                                        
   lcd.print("H:");                       //Printing Humidity
   dtostrf(Humidity, 4, 1, humStr);
   lcd.print(humStr);
   lcd.print(" %"); 
   lcd.line(4);
   lcd.print("P:");
   dtostrf(Pressure, 5, 1, presStr);
   lcd.print(presStr);
   lcd.print(" hPa");
}

void displayGettingData()
{
  lcd.clear();
  lcd.print("Getting data");
}

void loop() { 
  if(counter == 60)                                 //Get new data every 10 minutes
    {
      counter = 0;
      displayGettingData();
      delay(1000);
      getWeatherData();
    }else
    {
      counter++;
      displayWeather(weatherLocation,weatherDescription);
      delay(5000);
      displayConditions(Temperature,Humidity,Pressure);
      delay(5000);
    }
  
  
}



