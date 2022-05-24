#include <RV8523.h>
//#inlcude <SenseBoxMCU.h> only use with blockly code and commend out senseBoxIO.h
#include <senseBoxIO.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_HDC1000.h> // http://librarymanager/All#Adafruit_HDC1000_Library
#define BUFF_MAX 32

const long intervalInterval = 60000;
long time_startInterval = 0;
long time_actualInterval = 0;

File Data;
Adafruit_HDC1000 hdc = Adafruit_HDC1000();

RV8523 rtc;
unsigned int recv_size = 0;
char recv[BUFF_MAX];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  File Data;
  Adafruit_HDC1000 hdc = Adafruit_HDC1000();

  Serial.println("Init RTC...");
  delay(200);
  rtc.begin();
  rtc.start();
  rtc.batterySwitchOver(1);

}
void loop()
{
  uint8_t sec, min, hour, day, month;
  uint16_t year;

  //Uhrzeit befindet sich in den einzelnen Variablen !
  rtc.get(&sec, &min, &hour, &day, &month, &year);

  time_startInterval = millis();


  if (time_startInterval > time_actualInterval + intervalInterval) {
  time_actualInterval = millis();
  Data = SD.open("Data.txt", FILE_WRITE);
    Data.print(String(year) + String("-"));
    if(month < 10){
      Data.print("0");
    }
    Data.print(String(month) + String("-"));
    if(day < 10){
      Data.print("0");
    }
    Data.print(String(day) + String("T"));
    if(hour < 10){
      Data.print("0");
    }
    Data.print(String(hour) + String(":"));
    if(min < 10){
      Data.print("0");
    } 
    Data.println(String(min) + String("+01:00,") + String(hdc.readTemperature()) + String(",") + String(hdc.readHumidity()));
  Data.close();
}
}
