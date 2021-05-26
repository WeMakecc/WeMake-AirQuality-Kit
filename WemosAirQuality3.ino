/*
 * Sketch for Air Quality KIT  Project: Metti in Circolo
   Hardware: Wemos D1 mini, SDS011 Dust sensor, 
   Temperature and Humidity sensor SHT21, charge/discharge board
   for LiPo battery
 *                                     
 *   Compiler IDE: LOLIN(WEMOS) D1 R2 & mini
 *   Cable: micro USB
 *   
 *   Paolo Bonelli 07/03/2021
 */
//  D1 SCL, D2 SCA
#define PinRX  D3  //  RX pin for serial transmission from SDS011
#define PinTX  D4  //  TX pin for serial transmission to SDS011 (not connected)
#define PowPin D6  //  Switch on the power of SDS011
#define PAYLOAD_LEN 8  // Message length from SDS011 
byte received[PAYLOAD_LEN];  // array for the message
byte  i = 0;
byte  ch = ' ';
byte  flag = 0;
float PM10 = 0;  // PM10 concentration from Serial output
float PM25 = 0;  // PM2.5 concentration from Serial output
float Temp = 0;
float Humi = 0;
byte  err = 0;
float vBat = 0;
long Period = 20000;
#include <Wire.h>
#include "Config.h"
#include "Constant.h"
//  Display 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);  //   (azzurro e verde)

//  SHT21
#include <Sodaq_SHT2x.h>   //  address 0x40
byte SHT21present = 0;
byte error = 0;


//  SDS011
#include <SoftwareSerial.h>  
SoftwareSerial mySerial(PinRX, PinTX);
////////////////////////////////////////////////////////////    
byte ECHO = 1;
byte TrasmettiAda = 1;
///////////////////////////////////////////////////////////
//
void setup() {
  delay(3000);
  //  Display
  lcd.init();      
  delay(1000);
  Serial.println("LCD OK");

// Print a message to the LCD.
  lcd.backlight();
  lcd.clear();
  lcd.print("Wemake");
  lcd.setCursor(0, 1);
  lcd.print("AirQuality KIT");
  delay(3000);
  
  Serial.begin(57600);
  delay(1000);
  pinMode(PowPin,OUTPUT);
  pinMode(PinRX,INPUT);
  digitalWrite(PowPin,LOW);  // Switch off the SDS011
  SensInit();
  mySerial.begin(9600);
  delay(1000);
  if(ECHO)Serial.println(" WemosAirQuality3.ino");
  //  WiFi connection
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect (true);
  WiFi.setAutoReconnect (true);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);
  Serial.println();
  Serial.print("Wait for WiFi... ");  
  lcd.clear();
  lcd.print("Wait for WiFi...");
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  if(ECHO){
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    delay(500);
  }
  lcd.setCursor(0,1);
  lcd.print("Connected");

//  Connecting to Adafruit
  if(TrasmettiAda){
     if(ECHO)Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
     io.connect();
   
  // wait for a connection
     while(io.status() < AIO_CONNECTED) {
         if(ECHO)Serial.print(".");
         delay(500);
     }
     if(ECHO){
        Serial.println();
        Serial.println("Adafruit IO connected");
     }
     lcd.clear();
     lcd.print("Server connect.");
     delay(2000);
  } 
  lcd.noBacklight();
}
/////////////////////////////////////////////////////////
void loop() {
  if(TrasmettiAda){
           io.run();
  }  
  digitalWrite(PowPin,HIGH);  // Switch on the sensor and LED
  lcd.clear();
  lcd.backlight();
  lcd.print("Sucking in air..");
  delay(20000);
  flag = 0;
  i = 0;
  byte Roger = Received();
      if(Roger){
        PM25 = float(received[2]*256.0 + received[1])/10.0;
        PM10 = float(received[4]*256.0 + received[3])/10.0;
        digitalWrite(PowPin,LOW);
        ReadSHT21();
        ReadBattery();
        Display();
        if(ECHO){    
           Serial.println(" PM10   PM2.5");
           Serial.print(PM10);Serial.print(" ");Serial.print(PM25);
           Serial.println();
           Serial.print(" Temp ");Serial.print(Temp,1);
           Serial.print(" Humi ");Serial.println(Humi,1);
           Serial.print(" Bat (V) ");Serial.println(vBat,1); 
           Serial.println();
        }
        if(TrasmettiAda){
            TransmitWiFi();
            lcd.clear();
            lcd.backlight();
            lcd.print("Data to Internet");
            delay(5000);
            lcd.clear();
            lcd.print("Waiting for");
            lcd.setCursor(0,1);lcd.print(Period/1000);lcd.print(" s");
        }    
      }
      delay(Period);
}
