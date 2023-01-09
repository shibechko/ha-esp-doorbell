#include <WiFi.h>
#include <WiFiManager.h>    
#include <ArduinoHA.h>
#include <Bounce2.h>
#include "horn.hpp"

#define HORN_PIN 33
#define SWITCH_PIN 17
#define DEFAULT_DURATION 2000
#define WIFI_SSID "DoorBellAP"
#define WIFI_PASS "012345678"
#define MQTT_SERVER "192.168.8.3"

WiFiClient wifiClient;

HADevice device;
HAMqtt mqtt(wifiClient, device);
HABinarySensor button("switch");
HASwitch mode("mute");
HANumber duration("duration");

Horn horn = Horn(HORN_PIN, SWITCH_PIN, DEFAULT_DURATION);

void modeChanged(bool state, HASwitch* sender) {
  sender->setState(state);
}

void durationChange(HANumeric number, HANumber *sender) {
  sender->setState(number);
  horn.set_duration(number.toInt32());
}

void onWifiConnect(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WIFI Got IP");
  mqtt.begin(MQTT_SERVER);
}

void onWifiDisconnect(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WIFI disconnect");
  mqtt.disconnect();
}

void setup() {
  Serial.begin(115200);
  horn.init();

  Serial.println("Init HomeAssistant entityes\n");
  byte mac[6];
  WiFi.macAddress(mac);
  device.setUniqueId(mac, sizeof(mac));
  device.setName("DoorBell Controller");
  device.setSoftwareVersion("1.0.0");

  button.setName("Button");
  button.setIcon("mdi:home");

  mode.setName("Mute");
  mode.setIcon("mdi:bell");
  mode.onCommand(modeChanged);
  mode.turnOff();

  duration.setName("Duration");
  duration.setMode(HANumber::ModeSlider);
  duration.setState(DEFAULT_DURATION, true);
  duration.setMin(500);
  duration.setMax(10000);
  duration.setStep(500);
  duration.setUnitOfMeasurement("msec");
  duration.onCommand(durationChange);

  Serial.println("Init wifi connection\n");
  WiFiManager wifiManager;
  wifiManager.autoConnect(WIFI_SSID, WIFI_PASS);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  WiFi.onEvent(onWifiConnect, SYSTEM_EVENT_ETH_GOT_IP);
  WiFi.onEvent(onWifiDisconnect, SYSTEM_EVENT_STA_DISCONNECTED);

  Serial.println("Start mqtt\n");
  mqtt.begin(MQTT_SERVER);
}

void loop() {
  mqtt.loop();
  horn.update();
}
