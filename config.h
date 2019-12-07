/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME    "oalpha"
#define IO_KEY         "905085e4da1640d38f0fa38f6c0c365d"

/******************************* WIFI **************************************/

#define WIFI_SSID       "T-2_2aaa85"
#define WIFI_PASS       "INNBOX2626004304"

// comment out the following two lines if you are using fona or ethernet
#include "AdafruitIO_WiFi.h"
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
