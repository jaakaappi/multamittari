#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "DHT.h"

#include "config.h"

#define DHTPIN 23
#define DHTTYPE DHT22

#define SOIL_MOISTURE_PIN 36
#define ANALOG_RESOLUTION 4096.0
#define WET_SOIL 1265
#define DRY_SOIL 3838

DHT dht(DHTPIN, DHTTYPE);
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

void setup_mqtt()
{
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect("multamittari", access_token, ""))
    {
      Serial.println("connected");
      mqttClient.subscribe("esp32/output");
    }
    else
    {
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
  dht.begin();
}

void readDht()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature();

  if (isnan(h) || isnan(t) || isnan(f))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));
}

void readSoilMoisture()
{
  // https://makersportal.com/blog/2020/5/26/capacitive-soil-moisture-calibration-with-arduino manual coefficients
  float voltage = (float(analogRead(SOIL_MOISTURE_PIN)) / 4095.0) * 3.3;
  float moisture = (1.0 / voltage) * 1.50 - 0.54;
  Serial.print(F("Soil moisture "));
  Serial.print(moisture);
  Serial.println(F("cm³/cm³"));
}

void loop()
{
  mqttClient.publish("v1/devices/me/telemetry", "{\"humidity\":0}");
  readDht();
  readSoilMoisture();
  delay(5000);
}