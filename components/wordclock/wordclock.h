#pragma once

#include "esphome/core/component.h"
#include "esphome/components/time/real_time_clock.h"
#include "esphome/components/light/light_state.h"
#include <FastLED.h>

namespace esphome {
namespace wordclock {

class Wordclock : public Component {
 public:
  void setup() override;
  void loop() override;

  void set_time(time::RealTimeClock *time) { time_ = time; }
  void set_light(light::LightState *light) { light_ = light; }
  void on_setled(int number, int red, int blue, int green);

 protected:
  time::RealTimeClock *time_{nullptr};
  light::LightState *light_{nullptr};

  int hour_ = -1;
  int minute_ = -1;
  int red_ = 124;
  int green_ = 124;
  int blue_ = 124;
  int brightness_ = 50;
};

}  // namespace wordclock
}  // namespace esphome
