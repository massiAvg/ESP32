/*
*         Created on  : 04/07/22
*         Last Update : 05/07/22
*/

#include "EspMQTTClient.h"
#include <ArduinoJson.h>

String topic = "myTopic/massiAvg";
StaticJsonDocument<200> msg;

EspMQTTClient client(
  "wifiSsid",
  "wifiPassword",
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
  

}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe(topic, [](const String & payload) {
    if(payload != NULL)
    {
      msg["message"]=payload;
      msg["time"] = 135248;

      serializeJsonPretty(msg,Serial); 
    }
  });
}

void loop()
{
  String messagetopub=Serial.readString();  
  client.loop();
 
  if(messagetopub!= NULL )
  {
    // Publish a message to "mytopic/test"
    client.publish(topic, messagetopub);
    
    msg["message"]=messagetopub;
    msg["time"] = 135248;

    serializeJsonPretty(msg,Serial); 
  }
  

}
