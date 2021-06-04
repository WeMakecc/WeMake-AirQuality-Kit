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
 *   Paolo Bonelli 31/05/2021
 */
//  D1 SCL, D2 SCA
#define PinRX  D3  //  RX pin for serial transmission from SDS011
#define PinTX  D4  //  TX pin for serial transmission to SDS011 (not connected)
#define PowPin D6  //  Switch on the power of SDS011
#define LEDpin D5  //  pin output for alarm LED
#define PAYLOAD_LEN 8  // Message length from SDS011 
#include <Wire.h>
#include "Config.h"
#include "Constant.h"

byte received[PAYLOAD_LEN];  // array for the message
byte  i =        0;
byte  ch =       ' ';
byte  flag =     0;
float PM10 =     0;  // PM10 concentration from Serial output
float PM25 =     0;  // PM2.5 concentration from Serial output
int   j =        0;
float sum10 =    0.0;
float sum25 =    0.0;
float Temp =     0;
float Humi =     0;
byte  err =      0;
float vBat =     0;
float TimeOut =  0;
float StartTimeOut = 0;

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
byte  ECHO = 1;          // = 1 it prints on serial monitor; = 0 don't print
byte  TrasmettiAda = 1;  // = 1 it transmits data to Adafruit server; =0 don't transmit
byte  Connesso = 1;      // = 1 it connects to the WiFi
long  Period =   20000;  // waiting time between two measures
float threshold= 70.0;   // ug/m3 limit for switching on the red LED 
int   nsample =  8;      //  number of samples of SDS011 to be averaged
///////////////////////////////////////////////////////////
//
void setup() {
  delay(5000);
  pinMode(LEDpin,OUTPUT);
  digitalWrite(LEDpin,LOW); 
  
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
  if(ECHO)Serial.println(" WemosAirQuality4.ino");
  
  //  WiFi connection
  if(Connesso){
    WiFi.mode(WIFI_STA);
    WiFi.setAutoConnect (true);
    WiFi.setAutoReconnect (true);
    WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);
    Serial.println();
    Serial.print("Wait for WiFi... ");  
    lcd.clear();
    lcd.print("Wait for WiFi...");
    StartTimeOut = millis();
    while (WiFiMulti.run() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
      TimeOut = millis()-StartTimeOut;
      if(TimeOut > 15000){
        TrasmettiAda = 0;
        Connesso = 0;
        break;
      }
    }
  }
  if(Connesso){
    if(ECHO){
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      delay(500);
    }
    lcd.setCursor(0,1);
    lcd.print("Connected");
  }else{
    lcd.setCursor(0,1);
    lcd.print("No Connected");
  }
  delay(2000);

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
  } 
  delay(2000);
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
  j = 0;
  sum10 = 0.;
  sum25 = 0.;
  while (1){
        byte Roger = Received();
        if(Roger){
          PM25 = float(received[2]*256.0 + received[1])/10.0;
          PM10 = float(received[4]*256.0 + received[3])/10.0;
          Serial.println(" PM2.5 PM10");
          Serial.print(PM25,1);Serial.print("  ");Serial.println(PM10,1);
          sum10 = sum10 + PM10;
          sum25 = sum25 + PM25;
          j++;
          if(j >= nsample){
            PM10 = sum10 / nsample; 
            PM25 = sum25 / nsample; 
            break;
          }
        }
   }  
   digitalWrite(PowPin,LOW);
   ReadSHT21();
   ReadBattery();
   Display();
   if(PM10 > threshold){
    Serial.println("!!!!Alarm!!!!");
        Alarm(1);
   }else{
        Alarm(0);
   }
   if(ECHO){    
           Serial.println(" PM2.5 PM10");
           Serial.print(PM25,1);Serial.print(" ");Serial.print(PM10,1);
           Serial.println();
           Serial.print(" Temp ");Serial.print(Temp,1);
           Serial.print(" Humi ");Serial.println(Humi,1);
           Serial.print(" Bat (V) ");Serial.println(vBat,1); 
           Serial.println();
           Serial.println();
   }
   if(TrasmettiAda){
            TransmitWiFi();
            lcd.clear();
            lcd.backlight();
            lcd.print("Data to Internet");
   }    
   lcd.clear();
   lcd.print("Waiting for");
   lcd.setCursor(0,1);lcd.print(Period/1000);lcd.print(" s");
   delay(5000);
   delay(Period);
}
