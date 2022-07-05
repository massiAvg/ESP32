/*
*         Created on  : 04/07/22
*         Last Update : 05/07/22
*/

#include "EspMQTTClient.h"
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define SOLAR_TIME        1
#define NOT_SOLAR_TIME    2         //Between March 27 and October 30
#define GMT(x)            x*3600    // x can be SOLAR_TIME or NOT_SOLAR_TIME

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

int flagPubFromSerial = 0;
String topic = "myTopic/massiAvg";
StaticJsonDocument<200> msg;

EspMQTTClient client(
  "TIM-46474485",
  "6AcfE59DSt5kQCkDE7XHy5At",
  "broker.hivemq.com",  // MQTT Broker server ip
  "MQTTUsername",       // Can be omitted if not needed
  "MQTTPassword",       // Can be omitted if not needed
  "massiAvg",           // Client name that uniquely identify your device
  1883                  // The MQTT port, default to 1883. this line can be omitted
);

void setup()
{
  Serial.begin(115200);

  // Optionnal functionnalities of EspMQTTClient : 
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  //client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  //client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
  
  timeClient.setTimeOffset(GMT(NOT_SOLAR_TIME));
}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  while(!timeClient.update())
  {
    timeClient.forceUpdate();
  }
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe(topic, [](const String & payload) {
    if(payload != NULL && flagPubFromSerial == 0)
    {
      
      String timeNow2 = timeClient.getFullFormattedTime();
      msg["message"]=payload;
      msg["time"] = timeNow2;

      serializeJsonPretty(msg,Serial); 
    }
  });
}

void loop()
{
  String messagetopub=Serial.readString();  
  client.loop();
  delay(2);

  if(messagetopub!= NULL )
  {
    while(!timeClient.update())
    {
      timeClient.forceUpdate();
    }
    String timeNow = timeClient.getFullFormattedTime();
    // Publish a message to "mytopic/test"
    client.publish(topic, messagetopub);
    //client.unsubscribe(topic);
    msg["message"]=messagetopub;
    msg["time"] = timeNow;

    flagPubFromSerial = 1;

    serializeJsonPretty(msg,Serial); 
  }
  else
    flagPubFromSerial = 0;

  

}
