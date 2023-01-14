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

Horn horn = Horn(DEFAULT_DURATION);

Bounce spin = Bounce();

void modeChanged(bool state, HASwitch* sender) {
  sender->setState(state);
}

void durationChange(HANumeric number, HANumber *sender) {
  sender->setState(number);
  horn.set_duration(number.toInt32());
}

void onWifiConnect(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WIFI Got IP, Start MQTT");
  mqtt.begin(MQTT_SERVER);
}

void onWifiDisconnect(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("WIFI disconnect");
  mqtt.disconnect();
}

void setup() {
  Serial.begin(115200);

  spin.attach(SWITCH_PIN, INPUT_PULLUP);
  pinMode(HORN_PIN, OUTPUT);

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
  duration.setCurrentState(DEFAULT_DURATION);
  duration.setMin(500);
  duration.setMax(10000);
  duration.setStep(500);
  duration.setUnitOfMeasurement("msec");
  duration.onCommand(durationChange);

  Serial.println("Init wifi connection\n");
  WiFiManager wifiManager;
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  WiFi.onEvent(onWifiConnect, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(onWifiDisconnect, SYSTEM_EVENT_STA_DISCONNECTED);
  wifiManager.autoConnect(WIFI_SSID, WIFI_PASS);
}

void loop() {
  mqtt.loop();
  horn.update();
  spin.update();

  if(spin.changed()) {
      if(spin.read() == LOW) {
          horn.ring();
      }
  }

  if(!mode.getCurrentState()) {
    digitalWrite(HORN_PIN, horn.get_state());
  } else {
    digitalWrite(HORN_PIN, LOW);
  }
  button.setState(horn.get_state());
}
