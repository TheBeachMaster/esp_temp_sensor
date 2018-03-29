#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <AfricasTalkingCloud.h>
#include <Arduino.h>

#define WLAN_SSID "SSID"
#define WLAN_PASSWORD "PASSWORD"
#define QOS 2
#define DELAY_INTERVAL 60 

#define ONE_WIRE_BUS 2  
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

float oldTemp;

WiFiClient w_client;

AfricasTalkingCloudClient client(w_client);

void wifiConnect();
void sendTeperature(float temp);

void setup() {
  Serial.begin(115200);
  wifiConnect();
  oldTemp = -1;
}

void loop() {
    float temp;
  
    while (!client.connected())
    {
      client.connect("ESP8266-0112", "<username>", "<password>");
      client.publish("<username>/ESP8266-0112/birth", "ESP8266-0112birth");
    }

    do {
      DS18B20.requestTemperatures(); 
      temp = DS18B20.getTempCByIndex(0);
      Serial.print("Temperature: ");
      Serial.println(temp);
    } while (temp == 85.0 || temp == (-127.0));
    
    if (temp != oldTemp)
    {
      sendTeperature(temp);
      oldTemp = temp;
    }
    int cnt = DELAY_INTERVAL;
    while(cnt--)
      delay(1000);
}

void wifiConnect()
{
    Serial.print("Connecting to Network");
    WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
}

void sendTeperature(float temp)
{  
   while(!client.connected() && !client.loop()) {
    Serial.println("Connection failed");
    wifiConnect(); 
    client.connect("ESP8266-0112", "<username>", "<password>");
  }
 char data[50];
 snprintf(data,10, "%ld", temp);
 client.publish("<username>/ESP8266-0112/readings/temp", data, QOS);
}

