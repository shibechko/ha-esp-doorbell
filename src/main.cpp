#include <WiFi.h>
#include <WiFiManager.h>    
#include <Bounce2.h>

#define HORN_PIN 31
#define SWITCH_PIN 17

const char *default_ap = "DoorBellAP";
const char *default_pass = "012345678";

Bounce bounce = Bounce();

void setup() {
  Serial.begin(151200);

  Serial.println("Init pins\n");
  pinMode(HORN_PIN, OUTPUT);
  digitalWrite(HORN_PIN, LOW);

  pinMode(SWITCH_PIN, INPUT_PULLUP);
  bounce.attach(SWITCH_PIN);
  bounce.interval(20);

  Serial.println("Init wifi connection\n");
  WiFiManager wifiManager;
  wifiManager.autoConnect(default_ap, default_pass);

}

void horn() {
  Serial.println("Horn\n");
  digitalWrite(HORN_PIN, HIGH);
  delay(4000);
  digitalWrite(HORN_PIN, LOW);
}

void loop() {
  bounce.update();

  if ( bounce.changed() ) {
    int switch_state = bounce.read();
    Serial.print("Bounce read state is ");
    Serial.print(switch_state);
    if(switch_state == LOW) {
      horn();
    }
  }
}
