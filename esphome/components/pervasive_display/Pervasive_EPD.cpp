#include "waveshare_epaper.h"
#include <bitset>
#include <cinttypes>
#include "esphome/core/application.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pervasive_epd {

static const char *const TAG = "pervasive_epd";



// clang-format on

Pervasive_EPD::Pervasive_EPD(Pervasive_EPD_Model model) : model_(model) {
}

void Pervasive_EPD::setup() {
  this->init_internal_(this->get_buffer_length_());
  this->setup_pins_();
  this->spi_setup();
  this->reset_();
  this->initialize();
}
void Pervasive_EPD::setup_pins_() {
  this->dc_pin_->setup();  // OUTPUT
  this->dc_pin_->digital_write(false);
  this->cs_pin_->setup();  // OUTPUT
  this->cs_pin_->digital_write(false);
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();  // OUTPUT
    this->reset_pin_->digital_write(true);
  }
  if (this->busy_pin_ != nullptr) {
    this->busy_pin_->setup();  // INPUT
  }
}
float Pervasive_EPD::get_setup_priority() const { return setup_priority::PROCESSOR; }
void Pervasive_EPD::command(uint8_t value) {
  this->dc_pin->digital_write(false);
  this->cs_pin->digital_write(false);
  this->write_byte(value);
  this->cs_pin->digital_write(true);
}
void Pervasive_EPD::cmd_data(uint8_t cmd, uint8_t data) {
    this->dc_pin->digital_write(false);
    this->cs_pin->digital_write(false);
    this->write_byte(cmd);
    this->dc_pin->digital_write(true);
    this->write_byte(data);
    this->cs_pin->digital_write(true);
}
void Pervasive_EPD::cmd_data(uint8_t cmd, const uint8_t* data, size_t size) {
    this->dc_pin->digital_write(false);
    this->cs_pin->digital_write(false);
    //delay
    this->write_byte(cmd);
    //delay
    this->dc_pin->digital_write(true);
    //delay
    for (size_t i = 0; i < size; i++)
    {
        this->write_byte(data[i]);
    }
    //delay
    this->cs_pin->digital_write(true);
    //delay
}
void Pervasive_EPD::cmd_data_fixed(uint8_t cmd, uint8_t data, size_t size) {
    this->dc_pin->digital_write(false);
    this->cs_pin->digital_write(false);
    //delay
    this->write_byte(cmd);
    //delay
    this->dc_pin->digital_write(true);
    //delay
    for (size_t i = 0; i < size; i++)
    {
        this->write_byte(data);
    }
    //delay
    this->cs_pin->digital_write(true);
    //delay
}

uint8_t Pervasive_EPD::readSPI3()
{
    uint8_t value = 0;

    pinMode(SCK, OUTPUT);
    pinMode(MOSI, INPUT);

    for (uint8_t i = 0; i < 8; ++i)
    {
        digitalWrite(SCK, HIGH);
        delayMicroseconds(1);
        value |= digitalRead(MOSI) << (7 - i);
        digitalWrite(SCK, LOW);
        delayMicroseconds(1);
    }

    return value;
}

void Pervasive_EPD::writeSPI3(uint8_t value)
{
    pinMode(SCK, OUTPUT);
    pinMode(MOSI, OUTPUT);

    for (uint8_t i = 0; i < 8; i++)
    {
        digitalWrite(MOSI, !!(value & (1 << (7 - i))));
        delayMicroseconds(1);
        digitalWrite(SCK, HIGH);
        delayMicroseconds(1);
        digitalWrite(SCK, LOW);
        delayMicroseconds(1);
    }
}
//void Pervasive_EPD::data(uint8_t value) {
//  this->start_data_();
//  this->write_byte(value);
//  this->end_data_();
//}
//
//// write a command followed by one or more bytes of data.
//// The command is the first byte, length is the total including cmd.
//void Pervasive_EPD::cmd_data(const uint8_t *c_data, size_t length) {
//  this->dc_pin_->digital_write(false);
//  this->enable();
//  this->write_byte(c_data[0]);
//  this->dc_pin_->digital_write(true);
//  this->write_array(c_data + 1, length - 1);
//  this->disable();
//}
bool Pervasive_EPD::busy_wait(bool state) {
    if (this->busy_pin_ == nullptr || !this->busy_pin_->digital_read()) {
        return true;
    }

    const uint32_t start = millis();

    while (this->busy_pin_->digital_read() != state) {
    {
        if (millis() - start > this->idle_timeout_()) {
            ESP_LOGE(TAG, "Timeout while displaying image!");
            return false;
        }
        delay(1);
    }
    return true;
}

void Pervasive_EPD::update() {
  this->do_update_();
  this->display();
}
void Pervasive_EPD::fill(Color color) {
  // flip logic
  const uint8_t fill = color.is_on() ? 0x00 : 0xFF;
  for (uint32_t i = 0; i < this->get_buffer_length_(); i++)
    this->buffer_[i] = fill;
}
void HOT Pervasive_EPD::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x >= this->get_width_internal() || y >= this->get_height_internal() || x < 0 || y < 0)
    return;

  const uint32_t pos = (x + y * this->get_width_controller()) / 8u;
  const uint8_t subpos = x & 0x07;
  // flip logic
  if (!color.is_on()) {
    this->buffer_[pos] |= 0x80 >> subpos;
  } else {
    this->buffer_[pos] &= ~(0x80 >> subpos);
  }
}

uint32_t Pervasive_EPD::get_frame_length_() {
    return this->get_width_controller() * this->get_height_internal() / 8u;
}  // just a black buffer
uint32_t Pervasive_EPD::get_buffer_length_() {
    return this->get_frame_length_() * 2;
  //return this->get_width_controller() * this->get_height_internal() / 8u;
}  // just a black buffer
//uint32_t WaveshareEPaperBWR::get_buffer_length_() {
//  return this->get_width_controller() * this->get_height_internal() / 4u;
//}  // black and red buffer
//uint32_t WaveshareEPaper7C::get_buffer_length_() {
//  return this->get_width_controller() * this->get_height_internal() / 8u * 3u;
//}  // 7 colors buffer, 1 pixel = 3 bits, we will store 8 pixels in 24 bits = 3 bytes

//void Pervasive_EPD::start_command_() {
//  this->dc_pin_->digital_write(false);
//  this->enable();
//}
//void Pervasive_EPD::end_command_() { this->disable(); }
//void Pervasive_EPD::start_data_() {
//  this->dc_pin_->digital_write(true);
//  this->enable();
//}
//void Pervasive_EPD::end_data_() { this->disable(); }
//void Pervasive_EPD::on_safe_shutdown() { this->deep_sleep(); }

// ========================================================
//                          Type A
// ========================================================

void Pervasive_EPD::initialize() {
  // Achieve display intialization
  this->init_display_();
  //// If a reset pin is configured, eligible displays can be set to deep sleep
  //// between updates, as recommended by the hardware provider
  //if (this->reset_pin_ != nullptr) {
  //  switch (this->model_) {
  //    // More models can be added here to enable deep sleep if eligible
  //    case WAVESHARE_EPAPER_1_54_IN:
  //    case WAVESHARE_EPAPER_1_54_IN_V2:
  //      this->deep_sleep_between_updates_ = true;
  //      ESP_LOGI(TAG, "Set the display to deep sleep");
  //      this->deep_sleep();
  //      break;
  //    default:
  //      break;
  //  }
  //}
}
void Pervasive_EPD::init_display_() {
    spi_teardown();

    // Read OTP

    // Application note � 3. Read OTP memory
    // Register 0x50 flag
    // Additional settings for fast update, 154 206 213 266 271A 370 and 437 screens (s_flag50)
    switch (model_)
    {
    case EPD_154_KS_0C:
    case EPD_206_KS_0E:
    case EPD_213_KS_0E:
    case EPD_266_KS_0C:
    case EPD_271_KS_0C: // 2.71(A)
    case EPD_370_KS_0C:
    case EPD_437_KS_0C:

        s_flag50 = true;
        break;

    default:

        s_flag50 = false;
        break;
    }

    // Screens with no OTP
    switch (model_)
    {
    case EPD_150_KS_0J:
    case EPD_152_KS_0J:
    case EPD_290_KS_0F:

        ESP_LOGI("OTP check passed - embedded PSR");
        return; // No PSR
        break;

    default:

        break;
    }

    this->dc_pin_->digital_write(true);
    this->reset_pin_->digital_write(true);
    this->cs_pin_->digital_write(true);

    //b_reset(0, 5, 5, 10, 20);
    reset_();
    uint8_t ui8 = 0;
    uint16_t offsetA5 = 0x0000;
    uint16_t offsetPSR = 0x0000;
    uint16_t u_readBytes = 2;

    this->dc_pin_->digital_write(false); // Command
    this->cs_pin_->digital_write(false); // Select
    writeSPI3(0xa2);
    this->cs_pin_->digital_write(true); // Unselect
    delay(10);

    this->dc_pin_->digital_write(true); // Data
    this->cs_pin_->digital_write(false); // Select
    ui8 = readSPI3(); // Dummy
    this->cs_pin_->digital_write(true); // Unselect

    this->cs_pin_->digital_write(false); // Select
    ui8 = readSPI3(); // First byte to be checked
    this->cs_pin_->digital_write(true); // Unselect

    // Check bank
    uint8_t bank = ((ui8 == 0xa5) ? 0 : 1);

    switch (model_)
    {
    case EPD_271_KS_09:

        offsetPSR = 0x004b;
        offsetA5 = 0x0000;

        if (bank > 0)
        {
            COG_data[0] = 0xcf;
            COG_data[1] = 0x82;
            return;
        }
        break;

    case EPD_154_KS_0C:
    case EPD_266_KS_0C:
    case EPD_271_KS_0C: // 2.71(A)
    case EPD_370_KS_0C:
    case EPD_437_KS_0C:

        offsetPSR = (bank == 0) ? 0x0fb4 : 0x1fb4;
        offsetA5 = (bank == 0) ? 0x0000 : 0x1000;
        break;

    case EPD_206_KS_0E:
    case EPD_213_KS_0E:

        offsetPSR = (bank == 0) ? 0x0b1b : 0x171b;
        offsetA5 = (bank == 0) ? 0x0000 : 0x0c00;
        break;

    case EPD_417_KS_0D:

        // COG_data[0] = 0xcf;
        // COG_data[1] = 0x82;
        // return;


        offsetPSR = (bank == 0) ? 0x0b1f : 0x171f;
        offsetA5 = (bank == 0) ? 0x0000 : 0x0c00;
        break;

    default:
        Serial.println();
        ESP_LOGE("OTP check failed - Screen %i not supported", model_);
        exit(0x01);
        break;
    }

    // Check second bank
    if (offsetA5 > 0x0000)
    {
        for (uint16_t index = 1; index < offsetA5; index += 1)
        {
            this->cs_pin_->digital_write(false); // CS low = Select
            ui8 = readSPI3();
            this->cs_pin_->digital_write(true); // CS high = Unselect
        }

        this->cs_pin_->digital_write(false); // CS low = Select
        ui8 = readSPI3(); // First byte to be checked
        this->cs_pin_->digital_write(true); // CS high = Unselect

        if (ui8 != 0xa5)
        {
            Serial.println();
            ESP_LOGE("OTP check failed - Bank %i, first 0x%02x, expected 0x%02x", bank, ui8, 0xa5);
            exit(0x01);
        }
    }

    switch (this->model_)
    {
    case EPD_271_KS_09:

        ESP_LOGI("OTP check passed - Bank %i, first 0x%02x %s", bank, ui8, (bank == 0) ? "as expected" : "not checked");
        break;

    default:

        ESP_LOGI("OTP check passed - Bank %i, first 0x%02x as expected", bank, ui8);
        break;
    }

    for (uint16_t index = offsetA5 + 1; index < offsetPSR; index += 1)
    {
        this->cs_pin_->digital_write(false); // Select
        ui8 = readSPI3();
        this->cs_pin_->digital_write(true); // Unselect
    }
    // Populate COG_initialData
    for (uint16_t index = 0; index < u_readBytes; index += 1)
    {
        this->cs_pin_->digital_write(false); // Select
        COG_data[index] = readSPI3(); // Read OTP
        this->cs_pin_->digital_write(true); // Unselect
    }








  //if (this->model_ == TTGO_EPAPER_2_13_IN_B74 || this->model_ == WAVESHARE_EPAPER_2_13_IN_V2) {
  //  if (this->reset_pin_ != nullptr) {
  //    this->reset_pin_->digital_write(false);
  //    delay(10);
  //    this->reset_pin_->digital_write(true);
  //    delay(10);
  //    this->wait_until_idle_();
  //  }

  //  this->command(0x12);  // SWRESET
  //  this->wait_until_idle_();
  //}

  //// COMMAND DRIVER OUTPUT CONTROL
  //this->command(0x01);
  //this->data(this->get_height_internal() - 1);
  //this->data((this->get_height_internal() - 1) >> 8);
  //this->data(0x00);  // ? GD = 0, SM = 0, TB = 0

  //// COMMAND BOOSTER SOFT START CONTROL
  //this->command(0x0C);
  //this->data(0xD7);
  //this->data(0xD6);
  //this->data(0x9D);

  //// COMMAND WRITE VCOM REGISTER
  //this->command(0x2C);
  //this->data(0xA8);

  //// COMMAND SET DUMMY LINE PERIOD
  //this->command(0x3A);
  //this->data(0x1A);

  //// COMMAND SET GATE TIME
  //this->command(0x3B);
  //this->data(0x08);  // 2µs per row

  //// COMMAND DATA ENTRY MODE SETTING
  //this->command(0x11);
  //switch (this->model_) {
  //  case TTGO_EPAPER_2_13_IN_B1:
  //    this->data(0x01);  // x increase, y decrease : as in demo code
  //    break;
  //  case TTGO_EPAPER_2_13_IN_B74:
  //  case WAVESHARE_EPAPER_2_9_IN_V2:
  //    this->data(0x03);  // from top left to bottom right
  //    // RAM content option for Display Update
  //    this->command(0x21);
  //    this->data(0x00);
  //    this->data(0x80);
  //    break;
  //  default:
  //    this->data(0x03);  // from top left to bottom right
  //}
}

void Pervasive_EPD::dump_config() {
  LOG_DISPLAY("", "Waveshare E-Paper", this);
  switch (this->model_) {
    case WAVESHARE_EPAPER_1_54_IN:
      ESP_LOGCONFIG(TAG, "  Model: 1.54in");
      break;
    case WAVESHARE_EPAPER_1_54_IN_V2:
      ESP_LOGCONFIG(TAG, "  Model: 1.54inV2");
      break;
    case WAVESHARE_EPAPER_2_13_IN:
      ESP_LOGCONFIG(TAG, "  Model: 2.13in");
      break;
    case WAVESHARE_EPAPER_2_13_IN_V2:
      ESP_LOGCONFIG(TAG, "  Model: 2.13inV2");
      break;
    case TTGO_EPAPER_2_13_IN:
      ESP_LOGCONFIG(TAG, "  Model: 2.13in (TTGO)");
      break;
    case TTGO_EPAPER_2_13_IN_B73:
      ESP_LOGCONFIG(TAG, "  Model: 2.13in (TTGO B73)");
      break;
    case TTGO_EPAPER_2_13_IN_B74:
      ESP_LOGCONFIG(TAG, "  Model: 2.13in (TTGO B74)");
      break;
    case TTGO_EPAPER_2_13_IN_B1:
      ESP_LOGCONFIG(TAG, "  Model: 2.13in (TTGO B1)");
      break;
    case WAVESHARE_EPAPER_2_9_IN:
      ESP_LOGCONFIG(TAG, "  Model: 2.9in");
      break;
    case WAVESHARE_EPAPER_2_9_IN_V2:
      ESP_LOGCONFIG(TAG, "  Model: 2.9inV2");
      break;
  }
  ESP_LOGCONFIG(TAG, "  Full Update Every: %" PRIu32, this->full_update_every_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  LOG_PIN("  Busy Pin: ", this->busy_pin_);
  LOG_UPDATE_INTERVAL(this);
}

void HOT Pervasive_EPD::display() {

    bool full_update = this->at_update_ == 0;
    bool prev_full_update = this->at_update_ == 1;

    //b_resume(); // GPIO
    reset_();

    // Start SPI
    spi_setup();

    //COG_initial

    // Application note § 4. Input initial command
    switch (this->model_)
    {
    case EPD_150_KS_0J:
    case EPD_152_KS_0J:

        // Soft reset
        command(0x12);
        this->dc_pin_->digital_write(false);
        busy_wait(LOW); // 150 and 152 specific

        // Work settings
        cmd_data(0x1a, u_temperature);

        if (full_update)
        {
            cmd_data(0x22, 0xd7);
        }
        else
        {
            cmd_data(0x3c, 0xc0);
            cmd_data(0x22, 0xdf);
        }
        break;

    default:

        // Work settings
        uint8_t indexTemperature; // Temperature
        uint8_t index00_work[2]; // PSR

        // FILM_K already checked
        if (!full_update) // Specific settings for fast update
        {
            indexTemperature = u_temperature | 0x40; // temperature | 0x40
            index00_work[0] = COG_data[0] | 0x10; // PSR0 | 0x10
            index00_work[1] = COG_data[1] | 0x02; // PSR1 | 0x02
        }
        else // Common settings
        {
            indexTemperature = u_temperature; // Temperature
            index00_work[0] = COG_data[0]; // PSR0
            index00_work[1] = COG_data[1]; // PSR1
        } // u_codeExtra updateMode

        // New algorithm
        cmd_data(0x00, 0x0e); // Soft-reset
        busy_wait();

        cmd_data(0xe5, indexTemperature); // Input Temperature
        cmd_data(0xe0, 0x02); // Activate Temperature
        switch (this->model_)
        {
        case EPD_290_KS_0F: // No PSR

            cmd_data(0x4d, 0x55);
            cmd_data(0xe9, 0x02);
            break;

        default:

            cmd_data(0x00, index00_work, 2); // PSR
            break;
        }


        // Specific settings for fast update, all screens
        // FILM_K already checked
        if (!full_update)
        {
            cmd_data(0x50, 0x07); // Vcom and data interval setting
        }
        break;
    }
    //
    uint8_t *firstFrame = buffer_;

    uint32_t sizeFrame = get_frame_length_();

    if (!full_update && !isFirstFrame) {
        //COG_sendImageDataFast(buffer1, buffer2, buffer1_size);

        uint8_t* secondFrame = buffer_ + sizeFrame;
        // firstFrame: New Image for NORMAL, Old Image for FAST
        // secondFrame: 0x00 data for NORMAL, New Image for FAST

        // Application note § 4. Input image to the EPD
        // cmd_data(0x10, s_newImage, sizeFrame); // First frame, blackBuffer
        // cmd_data(0x10, frame, sizeFrame); // First frame, blackBuffer

        // Send image data
        switch (u_eScreen_EPD)
        {
        case eScreen_EPD_150_KS_0J:
        case eScreen_EPD_152_KS_0J:

            cmd_data(0x24, secondFrame, sizeFrame); // Next frame, blackBuffer
            cmd_data(0x26, firstFrame, sizeFrame); // Previous frame, 0x00
            break;

        default:
            // Additional settings for fast update, 154 213 266 370 and 437 screens (s_flag50)
            if (s_flag50)
            {
                cmd_data(0x50, 0x27); // Vcom and data interval setting
            }

            cmd_data(0x10, secondFrame, sizeFrame); // First frame, blackBuffer
            cmd_data(0x13, firstFrame, sizeFrame); // Second frame, 0x00

            // Additional settings for fast update, 154 213 266 370 and 437 screens (s_flag50)
            if (s_flag50)
            {
                cmd_data(0x50, 0x07); // Vcom and data interval setting
            }
            break;
        } // u_eScreen_EPD
    }
    else {
        //COG_sendImageDataNormal(buffer1, buffer1_size);

    // firstFrame: New Image for NORMAL, Old Image for FAST
    // secondFrame: 0x00 data for NORMAL, New Image for FAST

    // Application note § 4. Input image to the EPD
    // cmd_data(0x10, s_newImage, sizeFrame); // First frame, blackBuffer
    // cmd_data(0x10, frame, sizeFrame); // First frame, blackBuffer

    // Send image data
        switch (u_eScreen_EPD)
        {
        case eScreen_EPD_150_KS_0J:
        case eScreen_EPD_152_KS_0J:

            cmd_data(0x24, firstFrame, sizeFrame); // Next frame, blackBuffer
            cmd_data_fixed(0x26, 0x00, sizeFrame); // Previous frame, 0x00
            break;

        default:

            cmd_data(0x10, firstFrame, sizeFrame); // First frame, blackBuffer
            cmd_data_fixed(0x13, 0x00, sizeFrame); // Second frame, 0x00
            break;
        } // u_eScreen_EPD
    }

    //COG_update


    // Application note § 6. Send updating command
    switch (this->model_)
    {
    case EPD_150_KS_0J:
    case EPD_152_KS_0J:

        busy_wait(LOW); // 152 specific
        command(0x20); // Display Refresh
        this->cs_pin_->digital_write(true); // CS# = 1
        busy_wait(LOW); // 152 specific
        break;

    default:

        busy_wait();

        command(0x04); // Power on
        busy_wait();

        command(0x12); // Display Refresh
        busy_wait();
        break;
    }


    setBuffer2();

    COG_stopDCDC(); // Power off

}
int Pervasive_EPD::get_width_internal() {
    switch (this->model_)
    {
    case EPD_150_KS_0J:
    case EPD_152_KS_0J:
        return 200;
    case EPD_154_KS_0C:
        return 152;
    case EPD_206_KS_0E:
        return 128;
    case EPD_213_KS_0E:
        return 104;
    case EPD_266_KS_0C:
        return 152;
    case EPD_271_KS_09:
    case EPD_271_KS_0C:
        return 176;
    case EPD_290_KS_0F:
        return 168;
    case EPD_370_KS_0C:
        return 240;
    case EPD_417_KS_0D:
        return 300;
    case EPD_437_KS_0C:
        return 176;

    default:
        return 0;
    }
}
// The controller of the 2.13" displays has a buffer larger than screen size
int Pervasive_EPD::get_width_controller() {
    return this->get_width_internal();
}
int Pervasive_EPD::get_height_internal() {
    switch (this->model_)
    {
    case EPD_150_KS_0J:
    case EPD_152_KS_0J:
        return 200;
    case EPD_154_KS_0C:
        return 152;
    case EPD_206_KS_0E:
        return 248;
    case EPD_213_KS_0E:
        return 212;
    case EPD_266_KS_0C:
        return 296;
    case EPD_271_KS_09:
    case EPD_271_KS_0C:
        return 264;
    case EPD_290_KS_0F:
        return 384;
    case EPD_370_KS_0C:
        return 416;
    case EPD_417_KS_0D:
        return 400;
    case EPD_437_KS_0C:
        return 480;

    default:
        return 0;
    }
}
void Pervasive_EPD::write_lut_(const uint8_t *lut, const uint8_t size) {
  // COMMAND WRITE LUT REGISTER
  this->command(0x32);
  for (uint8_t i = 0; i < size; i++)
    this->data(lut[i]);
}
void Pervasive_EPD::set_full_update_every(uint32_t full_update_every) {
  this->full_update_every_ = full_update_every;
}

uint32_t Pervasive_EPD::idle_timeout_() {
  switch (this->model_) {
    case WAVESHARE_EPAPER_1_54_IN:
    case WAVESHARE_EPAPER_1_54_IN_V2:
    case WAVESHARE_EPAPER_2_13_IN_V2:
    case TTGO_EPAPER_2_13_IN_B1:
      return 2500;
    default:
      return Pervasive_EPD::idle_timeout_();
  }
}

}  // namespace waveshare_epaper
}  // namespace esphome
