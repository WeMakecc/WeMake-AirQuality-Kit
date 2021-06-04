/************************ Adafruit IO Config *******************************/
// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME  "nagecubic"
#define IO_KEY       "aio_EiZX7901W4K4INtv13RSH6jqkEKl"
/******************************* WIFI **************************************/

#define WIFI_SSID   "Wind3"
#define WIFI_PASS   "cicciobello445"


#include "AdafruitIO_WiFi.h"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
