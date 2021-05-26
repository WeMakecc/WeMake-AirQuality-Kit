#include <ESP8266WiFi.h>  // commentare se si usa sparkfun
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;
// set up the feeds
AdafruitIO_Feed *pm10 = io.feed("pm10");
AdafruitIO_Feed *pm25 = io.feed("pm25");
AdafruitIO_Feed *temp = io.feed("temp");
AdafruitIO_Feed *humi = io.feed("humi");
AdafruitIO_Feed *VBat = io.feed("VBat");
