/* 
 * Title: Tempurature and Humidity Sensing Clock
 * Author: Jesse Bertucci
 * Date: Aug 27, 2022
 */

#include <RTClib.h>            // DS3231 Clock module library 
#include <Wire.h>              // I2C Communication library for clock module
#include <LiquidCrystal.h>     // LCD display library
#include <Adafruit_Sensor.h>   // BME280 Tempurature, Pressure, Humidity, and Altitude module library 
#include <Adafruit_BME280.h>   // BME280 Tempurature, Pressure, Humidity, and Altitude module library 

#define BUTTON1 7                   // Initialize date and time setting buttons
#define BUTTON2 8                   // Initialize date and time setting buttons
#define LEDBACKLIGHT 6              // Set backlight to PWM pin to control backlight

Adafruit_BME280 bme;           // Delares a class of type Adafruit_BME280 called bme

RTC_DS3231 rtc;                // Declares a class of type RTC_DS3231 called rtc

int year, month, day, hour, minute;      // Global variables used to set the time by the user

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;    // Initializing LCD pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);                     // Initializing LCD 

int lightValue;                                                // Declares variable to store value from photoresistor
const int backlight = 120;                                     // Do not set higher, this variable controls how bright the LCD backlight will be on its maximum intensity 

const char monthInWords[13][4] = {" ", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};      // Used to display month name while setting time

byte degreeSymbol[8] = {                                       // Creates special 'degrees celsius' symbol
  0b10000,
  0b00111,
  0b01000,
  0b01000,
  0b01000,
  0b01000,
  0b00111,
  0b00000
};

void setup() {
  
  lcd.begin(16, 2);                                   // Starts LCD
	// Serial.begin(9600);                              // Starts the serial monitor, used for debugging. Uncomment to assist with debugging

  bme.begin(0x76);
  
  rtc.begin();                                        // Starts DS3231 clock module
  rtc.adjust(DateTime(2021, 12, 31, 23, 59, 0));      // Set a default time of 11:59pm, December 31st, 2021

  lcd.createChar(0, degreeSymbol);                    // Creates special 'degree celsius' symbol

  analogWrite(LEDBACKLIGHT, backlight);               // Initially sets backlight to max brightness

  pinMode(BUTTON1,INPUT);                             // Use button1 to enter into and scroll through date and time setting menu
  pinMode(BUTTON2,INPUT);                             // Use button2 to select the year, month, day, hour, and minute
  
}
 
void loop() {

  DateTime now = rtc.now();                          // Gets time from DS3231 module
  char buf1[] = "hh:mmap";                           // Defines time format HH:MM then am or pm
  char buf2[] = "MMM DD";                            // Defines date format MMM DD

  float hum = bme.readHumidity();                    // Get humidity
  float temp = bme.readTemperature();                // Get tempurature

  lcd.setCursor(0,0);                                // Set print area on LCD
  lcd.print(now.toString(buf1));                     // Get time in specified format
  lcd.print("|");
  lcd.print("T: ");
  lcd.print(temp, 1);                                // Display tempurature, limiting precision to 1 decimal place
  lcd.write(byte(0));                                // Display special degree symbol
  lcd.print("  ");
  lcd.setCursor(0,1);
  lcd.print(now.toString(buf2));                     // Display date in specified format
  lcd.print(" |");
  lcd.print("H: ");
  lcd.print(hum, 1);                                 // Displlay humidity, limiting precision to 1 decimal place
  lcd.print("% ");  

  lightValue = analogRead(A0);                       // Read value from photoresistor
  
  if(lightValue < 200){  
  analogWrite(LEDBACKLIGHT, 5);// Backlight OFF      // If the light in the bedroom is turned off, the LCD backlight intensity will decrease
  } else {
    analogWrite(LEDBACKLIGHT, backlight);            // If the light in the bedroom is turned on, LCD backlight intensity will increase
  }

  if(digitalRead(BUTTON1) == HIGH){                  // If button1 is depressed, date and time setting menu will be enter
    lcd.clear();
    lcd.print("Time Settings");
    delay(1000);        
    for(int i = 0; i < 5; i++){                      // Will cycle through five settings: year, month, day, hour, and minute
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Set date & time");
    lcd.setCursor(0,1);
      switch(i){                                     // Each iteration of the for loop will use a different setting
        case 0:
          year = now.year();                         // Begins with current year set
          lcd.print("Year: ");
          while(digitalRead(BUTTON1) == LOW){        // Depressing button1 will complete this setting and move onto the next
            lcd.setCursor(6, 1);
            lcd.print(year);
            if(digitalRead(BUTTON2) == HIGH){        // Button2 will increase the year
              year += 1;
            }
            if(year == 2099){                        // Max year is 2099, will cycle back to 2021 if exceeded
              year = 2021;                
            }
            delay(200);
          }
          break;
        case 1:
          delay(500);
          month = now.month();                       // Begins with current set month
          lcd.print("Month: ");
          while(digitalRead(BUTTON1) == LOW){        // Depressing button1 will complete this setting and move onto the next
            lcd.setCursor(7, 1);
            lcd.print(monthInWords[month]);
            if(digitalRead(BUTTON2) == HIGH){        // Button2 will increase the month
              month += 1;
            }
            if(month == 13){                         // If 12th month is exceeded, 1st month will be cycled back to 
              month = 1;
            }
            delay(200);
          }
          break;
        case 2:
          delay(500);
          day = now.day();                           // Begins with current set month
          lcd.print("Day: ");                
          while(digitalRead(BUTTON1) == LOW){        // Depressing button1 will complete this setting and move onto the next
            lcd.setCursor(5, 1);
            lcd.print("  ");
            lcd.setCursor(5, 1);
            lcd.print(day);
            if(digitalRead(BUTTON2) == HIGH){
              day += 1;
            }
            if(day == 32){                           // If 31st day is exceeded, 1st day will be cycled back to 
              day = 1;
            }
            delay(200);
          }
          break;
        case 3:   
          delay(500);
          hour = now.hour();                         // Begins with current set hour
          lcd.print("Hour: ");
          while(digitalRead(BUTTON1) == LOW){        // Depressing button1 will complete this setting and move onto the next
            lcd.setCursor(6, 1);
            lcd.print("    ");
            lcd.setCursor(6, 1);
            if(hour > 12){                           // Uses 12 hour am/pm time
              lcd.print(hour - 12);
              lcd.print("pm");
            } else {
              lcd.print(hour);
              lcd.print("am");
            }
            if(digitalRead(BUTTON2) == HIGH){
              hour += 1;
            }
            if(hour == 25){                          // If 24th hour is exceeded, 1st hour will be cycled back to 
              hour = 1;
            }
            delay(200);
          }
          break;
        case 4:   
          delay(500);
          minute = now.minute();                     // Begins with current set minute
          lcd.print("Minute: ");
          while(digitalRead(BUTTON1) == LOW){        // Depressing button1 will complete this setting and finish the date and time setting
            lcd.setCursor(8, 1);
            lcd.print("  ");
            lcd.setCursor(8, 1);
            lcd.print(minute);          
            if(digitalRead(BUTTON2) == HIGH){
              minute += 1;
            }
            if(minute == 60){                         // If 59th minute is exceeded, 1st minute will be cycled back to 
              minute = 0;
            }
            delay(200);
          }
          break;
      }
    }
    rtc.adjust(DateTime(year, month, day, hour, minute, 0));    // Sets the date and time the user selected
  }

/*
  Serial.print("Temp: ");                          // This section is used for debugging, to debug uncomment this and the "Serial.begin(9600);" in the setup function
  Serial.print(temp);
  Serial.println("Humidity: ");
  Serial.print(hum);
  Serial.println("Pressure: ");
  Serial.print(press);
  Serial.println("Light Value: ");
  Serial.print(lightValue);
  Serial.println("Time: ");  
  Serial.print(now.toString(buf1));  
  Serial.println("Date: ");
  Serial.print(now.toString(buf2));    
*/

  delay(1000);
}
