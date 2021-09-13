#ifndef pf
#define pf

#include "Arduino.h"
#include <ArduinoWebsockets.h>
#include <ESP8266WiFi.h>
    
class HearMe{
  public:
    
    HearMe();
    
    const char* ssid = "OPPO A7";                                               // Enter SSID
    const char* password = "12345678";                                          // Enter Password
    const char* websockets_server_host = "wss://dlsu-hearme.herokuapp.com";     // Enter server adress  

};

#endif
