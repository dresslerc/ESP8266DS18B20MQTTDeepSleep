
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_PIN D4

#define wifi_ssid "wifi_ssid"
#define wifi_password "wifi_password"

#define mqtt_server "192.168.1.240"
#define mqtt_user ""
#define mqtt_password ""

#define temperature_topic "sensor/temperature/"

OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);

  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while (!Serial) { }

  Serial.println("");
  Serial.print("Device ID: ");
  Serial.println(String(ESP.getChipId(), HEX));

  String(ESP.getChipId(), HEX);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  sensors.requestTemperatures();
  float temp = sensors.getTempFByIndex(0);

  Serial.print("Temperature: ");
  Serial.println(temp);
  String newTopic = temperature_topic + String(ESP.getChipId(), HEX);

  client.publish(newTopic.c_str(), String(temp).c_str(), true);

  client.loop();

  Serial.println("Sleep");

  ESP.deepSleep(60e6);
}

void setup_wifi() {
  delay(10);

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {

    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
