#include <graphicsLCD.h>

#define lcd_RST_pin 2
#define lcd_CS_pin 16
#define lcd_DIN_pin 18
#define lcd_CLK_pin 19

graphicsLCD lcd(lcd_RST_pin,lcd_CS_pin,lcd_DIN_pin,lcd_CLK_pin);

void setup() {
 
lcd.begin();
lcd.clear();

//inverse the display
lcd.displayInverse();
delay(3000);
// make the display normal
lcd.displayNormal();
delay(3000);


//change the contrast ratio to maximum
for( int darkness=0 ; darkness<32; darkness++)
{
  lcd.contrast(darkness);
  delay(100);
}

//change the contrast ratio to minimum
for( int darkness=32 ; darkness>1; darkness--)
{
  lcd.contrast(darkness);
  delay(100);
}

delay(5000);
lcd.displayAllPointsOn();
lcd.line(0);
delay(1000);
lcd.displayAllPointsOff();
delay(1000);

//jump between lines
lcd.clear();
lcd.line(0);
lcd.print("This is line 0");
delay(1000);
lcd.line(3);
lcd.print("This is line 3");
delay(1000);
lcd.line(7);
lcd.print("This is line 7");
delay(1000);

}

void loop() {

}

