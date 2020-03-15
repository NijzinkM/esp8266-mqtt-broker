#include <ESP8266WiFi.h>
#include "uMQTTBroker.h"

// WiFi config
#ifndef WIFI_NETWORK_NAME
  #define WIFI_NETWORK_NAME "***"
  #define WIFI_PASSWORD "***"
#endif

// AP config
#ifndef AP_NETWORK_NAME
  #define AP_NETWORK_NAME "***"
  #define AP_PASSWORD "***"
#endif

// MQTT config
#ifndef MQTT_USERNAME
  #define MQTT_USERNAME "***"
  #define MQTT_PASSWORD "***"
#endif

#define QOS 1

// Mac address of this device (optional)
String apMacAddress;

// Custom Broker class with overwritten callback functions
class myMQTTBroker: public uMQTTBroker {
public:
    virtual bool onConnect(IPAddress addr, uint16_t client_count) {
      Serial.println(addr.toString() + " connected");
      return true;
    }
    
    virtual bool onAuth(String username, String password) {
      Serial.println("Username/Password: " + username + "/********");

      return (username == MQTT_USERNAME && password == MQTT_PASSWORD);
    }
    
    virtual void onData(String topic, const char *data, uint32_t length) {
      char data_str[length + 1];
      os_memcpy(data_str, data, length);
      data_str[length] = '\0';
      
      Serial.println("received topic '" + topic + "' with data '" + String(data_str) + "'");
    }
};

myMQTTBroker myBroker;

void startWiFiClient() {
  Serial.println("Connecting to " + String(WIFI_NETWORK_NAME));
  WiFi.begin(WIFI_NETWORK_NAME, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());
  
  apMacAddress = WiFi.macAddress();
}

void startWiFiAP() {
  WiFi.softAP(AP_NETWORK_NAME, AP_PASSWORD, 1, false, 8); // name, password, channel, hidden, max clients
  Serial.println("AP started");
  Serial.println("IP address: " + WiFi.softAPIP().toString());
}

void setup() {
  Serial.begin(115200);
  pinMode(0, OUTPUT); // Initialize GIO0 pin as an output (optional)
  pinMode(2, INPUT); // Initialize GIO2 pin as an input (optional)

  startWiFiClient();
  startWiFiAP();
  
  // Start the broker
  Serial.println("Starting MQTT broker");
  myBroker.init();
}



void loop() {   
  myBroker.publish("broker/free_heap", (String) ESP.getFreeHeap(), QOS, false); // topic, data, QoS, retain
  myBroker.publish("broker/running_time", (String) millis(), QOS, false);
  delay(5000);
}
