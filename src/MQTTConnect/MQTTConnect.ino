#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems

char* topic= "myTopic/massiAvg"; // Definisco il topic


// L'SSID e la Password  vanno modificati per la propria rete.
const char* ssid = "TIM-46474485";
const char* password = "6AcfE59DSt5kQCkDE7XHy5At";
const char* mqtt_server = "broker.hivemq.com"; //broker pubblico 
#define mqtt_port 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
#define MQTT_SERIAL_PUBLISH_CH "HELLO ESP32"
#define MQTT_SERIAL_RECEIVER_CH "/icircuit/ESP32/serialdata/rx"


// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


#define SOLAR_TIME        1
#define NOT_SOLAR_TIME    2         //Between March 27 and October 30
#define GMT(x)            x*3600    // x can be SOLAR_TIME or NOT_SOLAR_TIME

// Variables to save date and time
String formattedDate;
String messagetopub;
String message;

WiFiClient wifiClient;

PubSubClient client(wifiClient);
//Setup della rete wifi
void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = "ESP32Client-";
   
   
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//Visualizzazione del topic sul canale.
void callback(char* topic, byte *payload, unsigned int length) {
  //In the callback() function, the ESP32 receives the MQTT messages of the subscribed topics.
    Serial.println("-------Nuovo messaggio: -----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("Messaggio:");  
    Serial.write(payload, length);
    Serial.println();
    
}

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
  Serial.begin(115200);
  Serial.setTimeout(500);// Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
          
  Serial.print("\n");
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(GMT(NOT_SOLAR_TIME)); // GMT(2)=7200
  
  }


void publishSerialData(char *serialData){
  if (!client.connected()) {
    reconnect();
  }
  client.publish(MQTT_SERIAL_PUBLISH_CH, serialData);
}

void loop() { 
  /*String messagetopub=Serial.readString();  
  client.loop();
  if (Serial.available() > 0) {
     char bfr[501];
     memset(bfr,0, 501);
     Serial.readBytesUntil( '\n',bfr,500);
     publishSerialData(bfr);
   }*/
  messagetopub="{\"clientID\":\"0001\",\"group\":\"massiAvg\",\"value\":\"12.5\", \"unit\":\"C\",\"time\":\"";
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  if(messagetopub!= NULL ){
    while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  
  formattedDate = timeClient.getFullFormattedTime();
   
  message = messagetopub + formattedDate + "\"" + "}";
  String clientId = "ESP32Client-";
  if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
     // Serial.println("connected");
      //Once connected, publish an announcement...
      client.publish(topic, message.c_str());
      // ... and resubscribe
      client.subscribe(topic);
      Serial.println("Messaggio Inviato");
      Serial.println(message.c_str());
      delay(5000);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
 }
}
 
