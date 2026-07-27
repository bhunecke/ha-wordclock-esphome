#pragma once

#include "esphome/core/component.h"
#include "esphome/components/time/real_time_clock.h"
#include "esphome/components/light/light_state.h"
#include "esphome/components/light/addressable_light.h"
#include "esphome/components/light/addressable_light_effect.h"

namespace esphome {
namespace wordclock {

class WordclockEffect : public light::AddressableLightEffect {
 public:
  WordclockEffect() : AddressableLightEffect("Wordclock Time") {}
  void set_time(time::RealTimeClock *time) { time_ = time; }
  void apply(light::AddressableLight &parent, const Color &current_color) override;
 protected:
  time::RealTimeClock *time_{nullptr};
  int last_log_minute_{-1};
};

class Wordclock : public Component {
 public:
  void setup() override;

  void set_time(time::RealTimeClock *time) { time_ = time; }
  void set_light(light::LightState *light) { light_ = light; }
  void on_setled(int number, int red, int blue, int green);

 protected:
  time::RealTimeClock *time_{nullptr};
  light::LightState *light_{nullptr};
  light::AddressableLight *led_strip_{nullptr};
  WordclockEffect effect_;
};

}  // namespace wordclock
}  // namespace esphome
