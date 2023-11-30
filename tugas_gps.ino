#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include<ESP8266WiFi.h>
#include<ESP8266HTTPClient.h>
#include<WiFiClient.h>

//gps things
const int RXPin = 3, TXPin = 1; //gpio digital pin 1 and 2
SoftwareSerial gpsSerial(RXPin, TXPin); // RX, TX ,pin 1-2
TinyGPSPlus gps; 
float latitude; float longitude; //make sure to make them double or float, just dont make them int

 const int led = 0;//for led pin

const uint32_t GPSBaud = 115200; //speed transfer data set to 9600
const uint32_t serial = 115200; //speed transfer data set to 9600

//for send to database
const char* ssid = "ZTE_2.4G_7r4Wtj";
const char* password = "Mt6TA9sK";
const char* serverName = "http://192.168.1.10/tugas_gps/send_data.php";
String apiKeyValue = "123456789";


void setup() {
  Serial.println("start");
  Serial.begin(serial);
  delay(1000); 
  gpsSerial.begin(GPSBaud);
  delay(1000);
  Serial.print("all begin done");
  
  //set up the wifi 
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("WIFI not connected :)");
  }
  Serial.println("");
  Serial.print("connected to ip : ");
  Serial.println(WiFi.localIP());
  Serial.println("----------------------------------------------");

  pinMode(led,OUTPUT);
}

/*
logic :
if gps ready that will take our location and send to database. then wait for 30 seconds and take our location again.
if not make the led blinking, if gps ready make the led keep on (just let us know if gps ready, my serial monitor always not show what i print. serial monitor always show weird words)
*/
void loop() {
    while (gpsSerial.available() > 0) {
      int gpsData = gpsSerial.read();
      Serial.println(gpsData);

      if (gps.encode(gpsSerial.read())) {
          if (gps.location.isValid()) {
            digitalWrite(led,HIGH);
            Serial.print("Latitude: ");
            //Serial.println(gps.location.lat(), 6);
            latitude = gps.location.lat();
            Serial.print("Longitude: ");
            //Serial.println(gps.location.lng(), 6);
            longitude = gps.location.lng();

            if(WiFi.status() == WL_CONNECTED)
            {
              WiFiClient wifiClient;
              HTTPClient http;
              String address;

              address = String(serverName);
              address += "?api_key=";
              address += String(apiKeyValue);
              address += "&latitude=";
              address += String(latitude);
              address += "&longitude=";
              address += String(longitude);


              http.begin(wifiClient, address);
              int statusCode = http.GET();

              if(statusCode > 0){
                Serial.print("HTTP Response Code : ");
                Serial.println(statusCode);
              }else{
                Serial.print("Error code : ");
                Serial.println(statusCode);
              }
              http.end();
              delay(30000);
            }
            else{
              Serial.println("Wifi not connected!!!");
            }
          }
          else{Serial.println("gps location not valid");}
      }
      else{Serial.println("failed to encode gps");}
    }
    Serial.println("gps not ready");
    digitalWrite(led,LOW);
    delay(2000);
    digitalWrite(led,HIGH);
    delay(500);

    
}
