#include "horn.hpp"

Horn::Horn(int hpin, int default_duration) {
    horn_pin = hpin; 
    duration = default_duration;
}

void Horn::ring() {
    start = millis();
}

bool Horn::get_state() {
    return state;
}

void Horn::update() {
    state = (millis()-start < duration) && start;
    if(state) {
        digitalWrite(horn_pin, HIGH);
    } else {
        digitalWrite(horn_pin, LOW);
    }
}

void Horn::set_duration(int dur) {
    duration = dur;
}

