#include "wordclock.h"
#include "esphome/core/log.h"

namespace esphome {
namespace wordclock {

static const char *const TAG = "wordclock";

#define NUM_LEDS 125

int leds_time_it_is[] = {0, 1, 3, 4, 5}; // ES IST
int leds_minutes[] = {124, 123, 122, 121}; // Minutes LEDS
int leds_skiped[] = {110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120}; // Check Line 101
int leds_time_minutes[][15] = {
    {101, 100,  99,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // UHR
    {  7,   8,   9,  10,  41,  40,  39,  38,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // FÜNF, NACH
    { 21,  20,  19,  18,  41,  40,  39,  38,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // ZEHN, NACH
    { 26,  27,  28,  29,  30,  31,  32,  41,  40,  39,  38,  -1,  -1,  -1,  -1}, // VIERTEL, NACH
    { 17,  16,  15,  15,  14,  13,  12,  11,  41,  40,  39,  38,  -1,  -1,  -1}, // ZWANZIG, NACH
    {  7,   8,   9,  10,  37,  36,  35,  44,  45,  46,  47,  -1,  -1,  -1,  -1}, // FÜNF, VOR, HALB
    { 44,  45,  46,  47,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // HALB
    {  7,   8,   9,  10,  41,  40,  39,  38,  44,  45,  46,  47,  -1,  -1,  -1}, // FÜNF, NACH, HALB
    { 17,  16,  15,  15,  14,  13,  12,  11,  37,  36,  35,  -1,  -1,  -1,  -1}, // ZWANZIG, VOR
    { 26,  27,  28,  29,  30,  31,  32,  37,  36,  35,  -1,  -1,  -1,  -1,  -1}, // VIERTEL, VOR
    { 21,  20,  19,  18,  37,  36,  35,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // ZEHN, VOR
    {  7,   8,   9,  10,  37,  36,  35,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}  // FÜNF, VOR
};

int leds_time_hours[][6] = {
    { 49,  50,  51,  52,  53,  -1}, // ZWÖLF
    { 63,  62,  61,  60,  -1,  -1}, // EINS
    { 65,  64,  63,  62,  -1,  -1}, // ZWEI
    { 67,  68,  69,  70,  -1,  -1}, // DREI
    { 80,  79,  78,  77,  -1,  -1}, // VIER
    { 73,  74,  75,  76,  -1,  -1}, // FÜNF
    {108, 107, 106, 105, 104,  -1}, // SECHS
    { 60,  59,  58,  57,  56,  55}, // SIEBEN
    { 89,  90,  91,  92,  -1,  -1}, // ACHT
    { 84,  83,  82,  81,  -1,  -1}, // NEUN
    { 93,  94,  95,  96,  -1,  -1}, // ZEHN
    { 87,  86,  85,  -1,  -1,  -1}  // ELF
};

void Wordclock::setup() {
  this->led_strip_ = static_cast<light::AddressableLight *>(light_->get_output());
  // Start all LED with on and default color and brightness to check if everything is working...
  for (int i = 0; i < NUM_LEDS; i++) { (*this->led_strip_)[i] = Color(red_, 0, 0); this->led_strip_->write_state(light_); delay(10); }
  for (int i = 0; i < NUM_LEDS; i++) { (*this->led_strip_)[i] = Color(0, green_, 0); this->led_strip_->write_state(light_); delay(10); }
  for (int i = 0; i < NUM_LEDS; i++) { (*this->led_strip_)[i] = Color(0, 0, blue_); this->led_strip_->write_state(light_); delay(10); }
  for (int i = 0; i < NUM_LEDS; i++) { (*this->led_strip_)[i] = Color(0, 0, 0); }
  this->led_strip_->write_state(light_);
}

void Wordclock::on_setled(int number, int red, int blue, int green) {
  if (number < NUM_LEDS && number >= 0) {
    ESP_LOGD(TAG, "Setting led number %d to color %i %i %i", number, red, green, blue);
    (*this->led_strip_)[number] = Color(red, green, blue);
    this->led_strip_->schedule_show();
  } else {
    ESP_LOGE(TAG, "Not a valid LED Number - out of range");
  }
}

void Wordclock::loop() {
  if (time_ == nullptr || light_ == nullptr) return;

  auto time = time_->now();
  int h = time.hour;
  int m = time.minute;

  auto light_values = light_->current_values;
  brightness_ = 0;

  int r = 0, g = 0, b = 0;
  if (light_values.get_state() > 0) {
    brightness_ = (int) (light_values.get_brightness() * 255);
    r = (int) (light_values.get_red() * light_values.get_brightness() * 255);
    g = (int) (light_values.get_green() * light_values.get_brightness() * 255);
    b = (int) (light_values.get_blue() * light_values.get_brightness() * 255);
  } else {
    ESP_LOGD(TAG, "wordclock light state off - b: %i rgb %i %i %i", brightness_, r, g, b);
    delay(500);
  }

  if (time.is_valid() == false) {
    ESP_LOGE(TAG, "Got invalid time from current_time Time: %i:%i", h, m);
    (*this->led_strip_)[0] = Color(255, 0, 0); this->led_strip_->write_state(light_); delay(250);
    (*this->led_strip_)[0] = Color(0, 255, 0); this->led_strip_->write_state(light_); delay(250);
    (*this->led_strip_)[0] = Color(0, 0, 255); this->led_strip_->write_state(light_); delay(250);
    (*this->led_strip_)[0] = Color(0, 0, 0);   this->led_strip_->write_state(light_);
  } else {
    if (h != hour_ || m != minute_) {
      hour_ = h;
      minute_ = m;
      if (hour_ >= 0 && time.is_valid() == true) {
        int tmp_hour = hour_;
        int tmp_minute = (minute_ - (minute_ % 5));
        if (tmp_minute >= 25) { tmp_hour += 1; }
        tmp_minute = tmp_minute / 5;
        tmp_hour = tmp_hour % 12;
        int minutessum = minute_ % 5;

        for (int i = 0; i < NUM_LEDS; i++) {     if (i < 110 || i > 120) (*this->led_strip_)[i] = Color(0, 0, 0); }
        for (int i = 0; i < 5; i++) {            (*this->led_strip_)[leds_time_it_is[i]] = Color(r, g, b); }
        for (int i = 0; i < 15; i++) {           if (leds_time_minutes[tmp_minute][i] >= 0) { (*this->led_strip_)[leds_time_minutes[tmp_minute][i]] = Color(r, g, b); } }
        for (int i = 0; i < 6; i++) {
          int led_idx = leds_time_hours[tmp_hour][i];
          if (tmp_hour == 1 && tmp_minute == 0 && led_idx == 60) continue; // EIN UHR instead of EINS UHR
          if (led_idx >= 0) { (*this->led_strip_)[led_idx] = Color(r, g, b); }
        }
        for (int i = 0; i < minutessum; i++) {   (*this->led_strip_)[leds_minutes[i]] = Color(r, g, b); }

        this->led_strip_->schedule_show();
        ESP_LOGD(TAG, "Update Time: %i:%i  Brightness: %i RGB: %i-%i-%i", hour_, minute_, brightness_, r, g, b);
        ESP_LOGD(TAG, "Using tmp_hour: %i tmp_minute: %i minutessum: %i", tmp_hour, tmp_minute, minutessum);
      }
    }
  }
}

}  // namespace wordclock
}  // namespace esphome
