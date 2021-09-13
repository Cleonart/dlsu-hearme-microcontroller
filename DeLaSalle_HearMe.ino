#include "DeLaSalle_HearMe.h"

HearMe hear;

// Wifi Settings
const char* ssid = hear.ssid;                                         // Enter SSID
const char* password = hear.password;                                 // Enter Password
const char* websockets_server_host = hear.websockets_server_host;     // Enter server adress

using namespace websockets;
WebsocketsClient client;                                              // Declare Websocket

// SSL Certificate Fingerprint
const char ssl_fingerprint[] PROGMEM = "39 A9 C4 FE B1 7E 23 9E 2F 4D BB AC E8 D7 A3 4F CE 43 0E 7B";

// Pin Definition
const int s0 = D0;
const int s1 = D1;
const int s2 = D2;
const int s3 = D3;
const int SIG_pin = A0;

// Variable
int PdB; 

/* Events Callback */
void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
}

void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

void setup() {

  Serial.begin(115200);
  pinMode(13, OUTPUT);
  
  pinMode(s0, OUTPUT); 
  pinMode(s1, OUTPUT); 
  pinMode(s2, OUTPUT); 
  pinMode(s3, OUTPUT); 

  digitalWrite(s0, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s2, LOW);
  digitalWrite(s3, LOW);

  connectToWifi();
  connectToWebSocket();
  
}

void connectToWifi(){
  Serial.print("connecting.");
  WiFi.begin(ssid, password);       // Connect to wifi
  for(int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
    Serial.print(".");
    digitalWrite(D0, LOW);
    delay(1000);
    digitalWrite(D0, HIGH);
    delay(1000);
  }
  
  // Check if connected to wifi
  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("No Wifi!");
    return;
  }
}

void connectToWebSocket(){
  Serial.println("Connected to Wifi, Connecting to server.");
  client.setFingerprint(ssl_fingerprint);
  
  // Try to connect to Websockets server
  bool connected = client.connect(websockets_server_host);
  if(connected) {
    Serial.println("Connected!");
  }
  else {
    Serial.println("Not Connected!");
  }
  
  // run callback when messages are received
  client.onMessage(onMessageCallback);
  
  // run callback when events are occuring
  client.onEvent(onEventsCallback);
  
  // Send a message
  client.send("Hello Server");
  
  // Send a ping
  client.ping();
}

int calculateDecibel(){
  digitalWrite(s0,LOW);
  digitalWrite(s1,LOW);
  digitalWrite(s2,LOW);
  digitalWrite(s3,LOW);
  int adc = analogRead(SIG_pin);  
  int dB = (adc+83.2073) / 11.003;    // Convert ADC value to dB using Regression values
  return dB;                          // Return Successfully Processed Data
}

void loop(){  
  if(client.available()){
    client.poll();
    client.send(String(calculateDecibel()));
    delay(100);
  }
}

int readMux(int channel){
  int controlPin[] = {s0, s1, s2, s3};

  int muxChannel[16][4]={
    {0,0,0,0}, //channel 0
    {1,0,0,0}, //channel 1
    {0,1,0,0}, //channel 2
    {1,1,0,0}, //channel 3
    {0,0,1,0}, //channel 4
    {1,0,1,0}, //channel 5
    {0,1,1,0}, //channel 6
    {1,1,1,0}, //channel 7
    {0,0,0,1}, //channel 8
    {1,0,0,1}, //channel 9
    {0,1,0,1}, //channel 10
    {1,1,0,1}, //channel 11
    {0,0,1,1}, //channel 12
    {1,0,1,1}, //channel 13
    {0,1,1,1}, //channel 14
    {1,1,1,1}  //channel 15
  };

  //loop through the 4 sig
  for(int i = 0; i < 4; i ++){
    digitalWrite(controlPin[i], muxChannel[channel][i]);
  }

  //read the value at the SIG pin
  int val = analogRead(SIG_pin);

  return val;
}
