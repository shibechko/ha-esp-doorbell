#include <Arduino.h>
#include <Bounce2.h>

class Horn {
  public:
    Horn(int hpin, int spin, int default_duration);
    void init();
    void update();
    void set_duration(int dur);    

  private:
    int duration;
    int horn_pin;
    int switch_pin;
    unsigned long start = 0;
    bool state = false;
    Bounce button;
};
