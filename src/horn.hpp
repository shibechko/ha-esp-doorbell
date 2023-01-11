#include <Arduino.h>

class Horn {
  public:
    Horn(int default_duration);
    void ring();
    bool get_state();
    void update();
    void set_duration(int dur);    

  private:
    int duration;
    int horn_pin;
    unsigned long start = 0;
    bool state = false;
};
