/*
  LiquidCrystal Library - Hello World

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch prints "Hello World!" to the LCD
 and shows the time.

  The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 modified 7 Nov 2016
 by Arturo Guadalupi

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/LiquidCrystalHelloWorld

*/

// include the library code:
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

SoftwareSerial gtSerial(0, 1); // Arduino RX, Arduino TX

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  
  lcd.setCursor(0,0);
  lcd.print("Bitte legen Sie");
  lcd.setCursor(0,1);
  lcd.print("Ihre Karte an");
  
  Serial.begin(9600);    // serial / USB port (python-script on rasp is listening on baud 115200), do not change this!
  gtSerial.begin(9600);  // software serial port
  Serial.println("Started all serials.");
}

void loop() {
  
  byte rx_byte = 0; 
  int loopCounter = 0;
  byte messageBuf[250];

  bool isSecondLine = false;
  bool isThirdLine = false; // Is true if there is a uid -> Card info has been sent.
  
  int firstLineLen = 0;
  int secondLineLen = 0;
  int thirdLineLen = 0;
  byte firstLineBuf[100];
  byte secondLineBuf[100];
  byte thirdLineBuf[100];

  rx_byte = gtSerial.read();
  
  if(rx_byte != 255){
    lcd.clear();
    while(rx_byte != 255){
      delay(5); // No idea why this is needed.. Sometimes the data isnt red fully, so everytime we read a byte, there has to be a small delay.
      if(rx_byte == 59){ // if ;
        if(isSecondLine){
          isThirdLine = true;
        }
        isSecondLine = true;
        rx_byte = gtSerial.read();
        continue;
      }
      if(isThirdLine){
        thirdLineBuf[thirdLineLen] = rx_byte;
        thirdLineLen++;
      }
      else if(isSecondLine){
        secondLineBuf[secondLineLen] = rx_byte;
        secondLineLen++;
      }else{
        firstLineBuf[firstLineLen] = rx_byte;
        firstLineLen++;
      }
      loopCounter++;
      rx_byte = gtSerial.read();
    }

    byte firstLine[firstLineLen];
    byte secondLine[secondLineLen];
    byte thirdLine[thirdLineLen];

    for(int i=0;i<firstLineLen;i++){
      firstLine[i] = firstLineBuf[i];
    }
    
    for(int i=0;i<secondLineLen;i++){
      secondLine[i] = secondLineBuf[i];
    }

    for(int i=0;i<thirdLineLen;i++){
      thirdLine[i] = thirdLineBuf[i];
    }

    
    //**********SEND DATA TO PI**********
    for(int i=0;i<firstLineLen;i++){
      Serial.print((char)firstLine[i]);
    }
    Serial.println("");

    if(isSecondLine){
      for(int i=0;i<secondLineLen;i++){
        Serial.print((char)secondLine[i]);
      }
    }
    Serial.println("");

    if(isThirdLine){
      for(int i=0;i<4;i++){
        Serial.print((char) thirdLine[i]);
      }
      for(int i=4;i<7;i++){
        Serial.print((char)thirdLine[i]);
      }
      for(int i=7;i<thirdLineLen;i++){
        Serial.print((char)thirdLine[i]);
      }
      Serial.println("");
    }
    Serial.println("Sent the data to the pi.");
    //***********************************

    //*********SHOW DATA ON LCD**********
    Serial.println("Showing data on screen.."); 
    lcd.setCursor(0,0);
    for(int i=0;i<firstLineLen;i++){
      lcd.print((char)firstLine[i]);
    }
    
    if(isSecondLine){
      lcd.setCursor(0,1);
      for(int i=0;i<secondLineLen;i++){
        lcd.print((char)secondLine[i]);
      }
    }
    delay(2500);
    lcd.clear();
    if(isThirdLine){
      lcd.setCursor(0,0);
      for(int i=0;i<16;i++){
        lcd.print((char) thirdLine[i]);
      }
      lcd.setCursor(0,1);
      for(int i=16;i<thirdLineLen;i++){
        lcd.print((char) thirdLine[i]);
      }
      lcd.print(" (Login)");
    }
    //***********************************

    delay(5000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Bitte legen Sie");
    lcd.setCursor(0,1);
    lcd.print("Ihre Karte an");
  }
}
