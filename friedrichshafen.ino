/*
  Test microSD

  Test progam for microSD card (mSD-Bee), connected to XBEE2 (SPI).
*/

#include <SPI.h>
#include <SD.h>
#include <senseBoxIO.h>
#include <RV8523.h>
#include <Wire.h>
#include <Adafruit_HDC1000.h> // http://librarymanager/All#Adafruit_HDC1000_Library
#define BUFF_MAX 32

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

File Data;
Adafruit_HDC1000 hdc = Adafruit_HDC1000();

RV8523 rtc;
unsigned int recv_size = 0;
char recv[BUFF_MAX];

void setup()
{
  // init serial library
  Serial.begin(9600);
  //while (!Serial); // wait for serial monitor
  Serial.println("Test microSD");

  // microSD in XBEE2 Socket
  senseBoxIO.powerXB2(false); // power off to reset microSD
  delay(250);
  senseBoxIO.powerXB2(true);  // power on
  senseBoxIO.SPIselectXB1();
  // init card
  const int chipSelect = PIN_XB1_CS;
  if (!card.init(SPI_HALF_SPEED, chipSelect))
  {
    Serial.println("Error - Not Found");
    senseBoxIO.statusRed();
    return; // don't continue
  }

  // print card type
  Serial.print("\nCard Type: ");
  switch (card.type())
  {
    case SD_CARD_TYPE_SD1:  Serial.println("SD1");     break;
    case SD_CARD_TYPE_SD2:  Serial.println("SD2");     break;
    case SD_CARD_TYPE_SDHC: Serial.println("SDHC");    break;
    default:                Serial.println("Unknown"); break;
  }

  // init volume/partition
  if (!volume.init(card))
  {
    Serial.println("Error - Not Found FAT16/FAT32 Partition");
    senseBoxIO.statusRed(); // status red
    senseBoxIO.powerXB2(false); // shutdown microSD
    return; // don't continue
  }

  // open/list volume/partition
  Serial.println("\nFiles:");
  root.openRoot(volume);
  root.ls(LS_R | LS_DATE | LS_SIZE);

  senseBoxIO.powerI2C(true);
  SD.begin(PIN_XB1_CS);

  Serial.println("Init RTC...");
  delay(200);
  rtc.begin();
  rtc.start();
  rtc.batterySwitchOver(1);


  Serial.println("Init HDC...");
  hdc.begin();


  // status green
  senseBoxIO.statusGreen();
}

void loop()
{
  uint8_t sec, min, hour, day, month;
  uint16_t year;

  // Uhrzeit befindet sich in den einzelnen Variablen !
  rtc.get(&sec, &min, &hour, &day, &month, &year);

  Data = SD.open("Data.txt", FILE_WRITE);
  Data.print(String(year) + String("-"));
  if (month < 10)
  {
    Data.print("0");
  }
  Data.print(String(month) + String("-"));
  if (day < 10)
  {
    Data.print("0");
  }
  Data.print(String(day) + String("T"));
  if (hour < 10)
  {
    Data.print("0");
  }
  Data.print(String(hour) + String(":"));
  if (min < 10)
  {
    Data.print("0");
  }
  Data.println(String(min) + String("+01:00,") + String(hdc.readTemperature()) + String(",") + String(hdc.readHumidity()));
  Data.close();

  Serial.print(String(year) + String("-"));
  if (month < 10)
  {
    Serial.print("0");
  }
  Serial.print(String(month) + String("-"));
  if (day < 10)
  {
    Serial.print("0");
  }
  Serial.print(String(day) + String("T"));
  if (hour < 10)
  {
    Serial.print("0");
  }
  Serial.print(String(hour) + String(":"));
  if (min < 10)
  {
    Serial.print("0");
  }
  Serial.println(String(min) + String("+01:00,") + String(hdc.readTemperature()) + String(",") + String(hdc.readHumidity()));
  delay(60000);
}
