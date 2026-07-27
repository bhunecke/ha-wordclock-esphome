#include "wordclock.h"
#include "esphome/core/log.h"

namespace esphome {
namespace wordclock {

static const char *const TAG = "wordclock";

int leds_time_it_is[] = {0, 1, 3, 4, 5}; // ES IST
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

const char *const log_leds[10][11] = {
    { "E", "S", "K", "I", "S", "T", "L", "F", "Ü", "N", "F" },
    { "Z", "E", "H", "N", "Z", "W", "A", "N", "Z", "I", "G" },
    { "D", "R", "E", "I", "V", "I", "E", "R", "T", "E", "L" },
    { "T", "G", "N", "A", "C", "H", "V", "O", "R", "J", "M" },
    { "H", "A", "L", "B", "Q", "Z", "W", "Ö", "L", "F", "P" },
    { "Z", "W", "E", "I", "N", "S", "I", "E", "B", "E", "N" },
    { "K", "D", "R", "E", "I", "R", "H", "F", "Ü", "N", "F" },
    { "E", "L", "F", "N", "E", "U", "N", "V", "I", "E", "R" },
    { "W", "A", "C", "H", "T", "Z", "E", "H", "N", "R", "S" },
    { "B", "S", "E", "C", "H", "S", "F", "M", "U", "H", "R" }
};

void Wordclock::setup() {
  this->led_strip_ = static_cast<light::AddressableLight *>(light_->get_output());
}

void Wordclock::on_setled(int number, int red, int blue, int green) {
  if (led_strip_ != nullptr && number < led_strip_->size() && number >= 0) {
    ESP_LOGD(TAG, "Setting led number %d to color %i %i %i", number, red, green, blue);
    (*this->led_strip_)[number] = Color(red, green, blue);
    this->led_strip_->schedule_show();
  } else {
    ESP_LOGE(TAG, "Not a valid LED Number - out of range");
  }
}

void Wordclock::loop() {
  if (time_ == nullptr || light_ == nullptr || led_strip_ == nullptr) return;

  auto &values = light_->current_values;
  if (values.get_state() <= 0) return;

  auto time = time_->now();
  if (!time.is_valid()) {
    (*this->led_strip_)[0] = Color(255, 0, 0);
    this->led_strip_->schedule_show();
    return;
  }

  int r = (int) (values.get_red() * values.get_brightness() * 255);
  int g = (int) (values.get_green() * values.get_brightness() * 255);
  int b = (int) (values.get_blue() * values.get_brightness() * 255);
  int w = (int) (values.get_white() * values.get_brightness() * 255);
  Color color(r, g, b, w);

  int h = time.hour;
  int m = time.minute;

  int tmp_hour = h % 12;
  int tmp_minute = (m - (m % 5)) / 5;
  if ((m % 5) >= 25) tmp_hour = (tmp_hour + 1) % 12;

  int n = this->led_strip_->size();
  Color off(0, 0, 0, 0);
  for (int i = 0; i < n; i++) {
    if (i < 110 || i > 120) (*this->led_strip_)[i] = off;
  }
  for (int i = 0; i < 5; i++) {
    (*this->led_strip_)[leds_time_it_is[i]] = color;
  }
  for (int i = 0; i < 15; i++) {
    if (leds_time_minutes[tmp_minute][i] >= 0) {
      (*this->led_strip_)[leds_time_minutes[tmp_minute][i]] = color;
    }
  }
  for (int i = 0; i < 6; i++) {
    int led_idx = leds_time_hours[tmp_hour][i];
    if (tmp_hour == 1 && tmp_minute == 0 && led_idx == 60) continue;
    if (led_idx >= 0) (*this->led_strip_)[led_idx] = color;
  }

  this->led_strip_->schedule_show();

  if (m != this->last_log_minute_) {
    this->last_log_minute_ = m;
    ESP_LOGD(TAG, "Time: %i:%i  RGBW: %i-%i-%i-%i  (tmp_hour: %i tmp_minute: %i)", h, m, r, g, b, w, tmp_hour, tmp_minute);
    for (int row = 0; row < 10; row++) {
      char line[32];
      int pos = 0;
      int base = row * 11;
      for (int col = 0; col < 11; col++) {
        int idx = (row % 2 == 0) ? base + col : base + (10 - col);
        auto led = (*this->led_strip_)[idx];
        const char *s = (led.get_red() + led.get_green() + led.get_blue() + led.get_white() > 0) ? log_leds[row][col] : "-";
        while (*s) line[pos++] = *s++;
      }
      line[pos] = '\0';
      ESP_LOGD(TAG, "%s", line);
    }
  }
}

}  // namespace wordclock
}  // namespace esphome
