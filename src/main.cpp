#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "config.h"

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Adapted from https://randomnerdtutorials.com/esp32-mqtt-publish-subscribe-arduino-ide/

void setup_wifi()
{
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_mqtt() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("multamittari", access_token, "")) {
      Serial.println("connected");
      mqttClient.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(9600);
  setup_wifi();
  mqttClient.setServer(mqtt_server, 1883);
  setup_mqtt();
}

void loop()
{
  mqttClient.publish("v1/devices/me/telemetry","{\"humidity\":0}");
  delay(5000);
}