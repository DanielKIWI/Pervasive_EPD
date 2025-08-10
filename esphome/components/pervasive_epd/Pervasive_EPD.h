#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/display/display_buffer.h"
//#include "../../../esphome/core/component.h"
//#include "../../../esphome/components/spi/spi.h"
//#include "../../../esphome/components/display/display_buffer.h"

namespace esphome {
namespace pervasive_epd {

enum Pervasive_EPD_Model {
    EPD_150_KS_0J = 0,
    EPD_152_KS_0J,
    EPD_154_KS_0C,
    EPD_206_KS_0E,
    EPD_213_KS_0E,
    EPD_266_KS_0C,
    EPD_271_KS_09,
    EPD_271_KS_0C,
    EPD_290_KS_0F,
    EPD_370_KS_0C,
    EPD_417_KS_0D,
    EPD_437_KS_0C,
};

public class Pervasive_EPD : public display::DisplayBuffer,
                            public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
                                                  spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_4MHZ> {
 public:

  Pervasive_EPD(Pervasive_EPD_Model model);

  void set_dc_pin(GPIOPin *dc_pin) { dc_pin_ = dc_pin; }
  void set_cs_pin(GPIOPin *cs_pin) { dc_pin_ = cs_pin; }
  float get_setup_priority() const override;
  void set_reset_pin(GPIOPin *reset) { this->reset_pin_ = reset; }
  void set_busy_pin(GPIOPin *busy) { this->busy_pin_ = busy; }
  void set_reset_duration(uint32_t reset_duration) { this->reset_duration_ = reset_duration; }

  void command(uint8_t value);
  void cmd_data(uint8_t cmd, uint8_t value);
  void cmd_data(uint8_t cmd, const uint8_t *data, size_t length);
  void cmd_data_fixed(uint8_t cmd, uint8_t data, size_t length);
  //void cmd_data(const uint8_t *data, size_t length);
  uint8_t readSPI3();
  void writeSPI3(uint8_t value);

  void display();
  void initialize();
  void dump_config() override;
  void deep_sleep();

  void update() override;

  void setup() override;

  void on_safe_shutdown() override;
  void set_full_update_every(uint32_t full_update_every);

 protected:
  bool busy_wait(bool state = HIGH);

  void setup_pins_();

  void reset_() {
    if (this->reset_pin_ != nullptr) {
      //this->reset_pin_->digital_write(false);
      //delay(reset_duration_);  // NOLINT
      //this->reset_pin_->digital_write(true);
      //delay(20);

      delay(1); // Wait for power stabilisation
      this->reset_pin_->digital_write(true);
      delay(5);
      this->reset_pin_->digital_write(false);
      delay(5);
      this->reset_pin_->digital_write(true);
      delay(10);
      this->cs_pin_->digital_write(true);
      delay(20);
    }

    //switch (u_eScreen_EPD)
    //{
    //case eScreen_EPD_150_KS_0J:
    //case eScreen_EPD_152_KS_0J:

    //    if (digitalRead(_busy_pin) == HIGH)
    //    {
    //        Serial.println();
    //        log(LEVEL_CRITICAL, "Incorrect type for 1.52-Wide");
    //        exit(0x01);
    //    }
    //    break;

    //default:

    //    break;
    //}
  }

  virtual int get_width_controller() { return this->get_width_internal(); };

  virtual uint32_t get_buffer_length_() = 0;  // NOLINT(readability-identifier-naming)
  uint32_t reset_duration_{200};


  GPIOPin *reset_pin_{nullptr};
  GPIOPin *dc_pin_;
  GPIOPin *cs_pin_;
  GPIOPin *busy_pin_{nullptr};
  virtual uint32_t idle_timeout_() { return 1000u; }  // NOLINT(readability-identifier-naming)
  uint8_t COG_data[2]; // OTP

 public:
  void fill(Color color) override;

  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_BINARY; }

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  uint32_t get_buffer_length_() override;


  void write_lut_(const uint8_t* lut, uint8_t size);

  void init_display_();

  int get_width_internal() override;

  int get_height_internal() override;

  int get_width_controller() override;

  uint32_t full_update_every_{ 30 };
  uint32_t at_update_{ 0 };
  Pervasive_EPD_Model model_;
  uint32_t idle_timeout_() override;

  bool deep_sleep_between_updates_{ false };
};

}  // namespace waveshare_epaper
}  // namespace esphome
