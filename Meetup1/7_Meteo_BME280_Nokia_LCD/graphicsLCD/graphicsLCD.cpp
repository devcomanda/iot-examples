/*
  Morse.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#include "Arduino.h"
#include "graphicsLCD.h"

graphicsLCD::graphicsLCD(byte ResetPin,byte CSPin,byte SDAPin,byte SCKPin)

{
  _CSPin = CSPin;
  _SCKPin  =SCKPin;
  _SDAPin =SDAPin;
  _ResetPin=ResetPin;

   //set pin directions
  pinMode(_CSPin, OUTPUT);
  pinMode(_SDAPin, OUTPUT);
  pinMode(_SCKPin, OUTPUT);
  pinMode(_ResetPin, OUTPUT);

  //disable graphicsLCD to start with 
  digitalWrite(_CSPin, HIGH);
  digitalWrite(_SDAPin, LOW);
  digitalWrite(_SCKPin, LOW);
  
  // set reset pin to start graphicsLCD
  digitalWrite(_ResetPin, HIGH);  
 }
 
 
void graphicsLCD::contrast(byte con)
{
  if(con<0) {con=0;}
  if(con>32) {con=32;}

con = con | 0x80;
graphicsLCD::command(con);  
}


void graphicsLCD::displayAllPointsOn()
{
  graphicsLCD::command(0xA5);
}


void graphicsLCD::displayAllPointsOff()
{
  graphicsLCD::command(0xA4);
}


void graphicsLCD::softwareReset()
{
  graphicsLCD::command(0xE2);  //reset
}


void graphicsLCD::hardwareReset()
{
  digitalWrite(_ResetPin, LOW);
  delay(5);
  digitalWrite(_ResetPin, HIGH);
  delay(5);
}

 
void graphicsLCD::line(byte line)
{
  if (line<0){line = 0;}
  if (line>8){line = 8;}
  
  line = 176+line;
  
  graphicsLCD::command(line);  //set line
/*
  graphicsLCD::command(0x10);  //set col=0 upper 3 bits
  graphicsLCD::command(0x00);  //set col=0 lower 4 bits
*/
  graphicsLCD::column(0x00);
}

 void graphicsLCD::column(byte payload)
{
if(payload<0){payload=0;}
if(payload>95){payload=95;}

byte lower = 0x0F;
byte upper = 0x70;

lower = payload & 0x0F;
upper =payload &0x70;

upper = upper>>4;
 
upper = 0x10 | upper;
 
graphicsLCD::command(upper);
graphicsLCD::command(lower);
}

void graphicsLCD::scroll(byte payload)
{
/*  payload = payload+64;
  if (payload >127){payload = 127;}
 */
  payload =64+ payload;
  
  if (payload <0){payload = 64;}
  if (payload >127){payload = 127;}
  graphicsLCD::command(payload);
}

void graphicsLCD::displayOff()
{
  graphicsLCD::command(0xAE);
}

void graphicsLCD::displayOn()
{
  graphicsLCD::command(0xAF);
}

void graphicsLCD::displayNormal()
{
  graphicsLCD::command(0xA6);
}

void graphicsLCD::displayInverse()
{
  graphicsLCD::command(0xA7);
}

 void graphicsLCD::println(char *characters)
{
  graphicsLCD::command(0x10);  //set column=0 upper 3 bits
  graphicsLCD::command(0x00);  //set column=0 lower 4 bits

  while (*characters)
  {
    graphicsLCD::character(*characters++);
  }
}


 
 void graphicsLCD::print(char *characters)
{
  while (*characters)
  {
    graphicsLCD::character(*characters++);
  }
}

 void graphicsLCD::print(long num)
{
char c[20];
ltoa(num,c,10);
graphicsLCD::print(c);
}

void graphicsLCD::println(long num)
{
char c[20];
ltoa(num,c,10);
graphicsLCD::println(c);
}


void graphicsLCD::character(char character)
{
  for (int index = 0; index < 5; index++)
  {
     graphicsLCD::data(ASCII[character - 0x20][index]);  
  
  }
  graphicsLCD::data(0x00);  
}


void graphicsLCD::clear()
{
  
  //display off
  graphicsLCD::command(0xAE);
  
  for (int i = 0; i < 864; i++)
  {
    graphicsLCD::data(0x00);  
  }

  graphicsLCD::command(0xB0);  //set page address
  graphicsLCD::command(0x10);  //set col=0 upper 3 bits
  graphicsLCD::command(0x00);  //set col=0 lower 4 bits

  graphicsLCD::command(0x40);  //set row 0

  //display on
  graphicsLCD::command(0xAF);

}

void graphicsLCD::command(byte payload)
{
  
  int dc =0 ;
  //select graphicsLCD
  digitalWrite(_CSPin, LOW);
  
  //write dc bit
  if(dc==0){digitalWrite(_SDAPin, LOW);}
  else {digitalWrite(_SDAPin, HIGH);}
  digitalWrite(_SCKPin, HIGH);
  digitalWrite(_SCKPin, LOW);
  
  //write payload 8 bits
  for(int i=7;i>=0;i--)
  {
      digitalWrite(_SDAPin, (payload>>i)&1);
      //toggle clock
      digitalWrite(_SCKPin, HIGH);
      digitalWrite(_SCKPin, LOW);
  }
  
  //turn off graphicsLCD cs
  digitalWrite(_CSPin, HIGH);
}

void graphicsLCD::begin()
{
  graphicsLCD::command(0xE2);  //reset
  delay(10);
  graphicsLCD::command(0xA4);  //power save off
  graphicsLCD::command(0x2F);  //power control set
  graphicsLCD::command(0xB0);  //set page address
  graphicsLCD::command(0x10);  //set col=0 upper 3 bits
  graphicsLCD::command(0x00);  //set col=0 lower 4 bits
  
  graphicsLCD::command(0xAF);  //graphicsLCD display on
  delay(500);
}

void graphicsLCD::data(byte payload)
{
  
  int dc =1 ;
  //select graphicsLCD
  digitalWrite(_CSPin, LOW);
  
  //write dc bit
  if(dc==0){digitalWrite(_SDAPin, LOW);}
  else {digitalWrite(_SDAPin, HIGH);}
  digitalWrite(_SCKPin, HIGH);
  digitalWrite(_SCKPin, LOW);
  
  //write payload 8 bits
  for(int i=7;i>=0;i--)
  {
      digitalWrite(_SDAPin, (payload>>i)&1);
      //toggle clock
      digitalWrite(_SCKPin, HIGH);
      digitalWrite(_SCKPin, LOW);
  }
  
  //turn off graphicsLCD cs
  digitalWrite(_CSPin, HIGH);
}
