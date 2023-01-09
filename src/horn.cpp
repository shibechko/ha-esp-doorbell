#include "horn.hpp"

Horn::Horn(int hpin, int spin, int default_duration) {
    horn_pin = hpin; 
    switch_pin = spin;
    duration = default_duration;
}

void Horn::init() {
    button = Bounce();
    button.attach(switch_pin, INPUT_PULLUP);
}

void Horn::update() {
    state = (millis()-start < duration) && start;
    if(state) {
        Serial.println(state);
        digitalWrite(horn_pin, HIGH);
    } else {
        digitalWrite(horn_pin, LOW);
    }
    button.update();
    if(button.changed()) {
        if(button.read() == LOW) {
            start = millis();
        }
    }
}

void Horn::set_duration(int dur) {
    Serial.println(dur);
    duration = dur;
}

