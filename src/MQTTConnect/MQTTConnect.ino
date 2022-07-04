/*
  SimpleMQTTClient.ino
  The purpose of this exemple is to illustrate a simple handling of MQTT and Wifi connection.
  Once it connects successfully to a Wifi network and a MQTT broker, it subscribe to a topic and send a message to it.
  It will also send a message delayed 5 seconds later.
*/

#include "EspMQTTClient.h"

String topic = "myTopic/test";

EspMQTTClient client(
  "WiFiSSID",
  "WiFiPassword",
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
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe(topic, [](const String & payload) {
    //Serial.println(payload);
  });
}

void loop()
{
  //client.loop();
  String messagetopub=Serial.readString();  
  client.loop();
  if(messagetopub!= NULL ){
         // Publish a message to "mytopic/test"
    client.publish(topic, messagetopub); // You can activate the retain flag by setting the third parameter to true
  }
  

}
