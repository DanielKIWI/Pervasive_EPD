#ifndef LIB_PERVASIVE_EPD
#define LIB_PERVASIVE_EPD

#include "Adafruit_EPD.h"
#include <Arduino.h>
//#include "PDLS_Common/hV_List_Constants.h"

//#define FAMILY_SMALL 0x01 ///< Small screens
//#define FAMILY_MEDIUM 0x02 ///< Medium-sized screens
//#define FAMILY_LARGE 0x03 ///< Large screens with two controllers

#define FSM_OFF 0x00 ///< Bus off, GPIO off or undefined
#define FSM_ON 0x11 ///< Bus on, GPIO on or defined
#define FSM_SLEEP 0x10 ///< Bus on, GPIO and Power off or undefined
#define FSM_GPIO_MASK 0x01 ///< Mask for GPIO on or defined
#define FSM_BUS_MASK 0x10 ///< Mask for bus on

#define SIZE_NONE 0x00 ///< Unknown
#define SIZE_150 150 ///< 1.50"
#define SIZE_152 152 ///< 1.52"
#define SIZE_154 154 ///< 1.54"
#define SIZE_206 206 ///< 2.06"
#define SIZE_213 213 ///< 2.13"
#define SIZE_215 215 ///< 2.15"
#define SIZE_266 266 ///< 2.66"
#define SIZE_271 271 ///< 2.71"
#define SIZE_287 287 ///< 2.87"
#define SIZE_290 290 ///< 2.90"
#define SIZE_340 340 ///< 3.40"
#define SIZE_343 343 ///< 3.43"
#define SIZE_350 350 ///< 3.50"
#define SIZE_370 370 ///< 3.70"
#define SIZE_417 417 ///< 4.17"
#define SIZE_437 437 ///< 4.37"
#define SIZE_565 565 ///< 5.65"
#define SIZE_581 581 ///< 5.81"
#define SIZE_741 741 ///< 7.41"
#define SIZE_969 969 ///< 9.69"
#define SIZE_B98 1198 ///< 11.98"
#define SIZE_1198 1198 ///< 11.98", synonym for SIZE_B98

#define FILM_NONE 0x00 ///< Unknown
#define FILM_C 'C' ///< Film C, Standard
#define FILM_E 'E' ///< Film E, BWR, deprecated
#define FILM_F 'F' ///< Film E, BWR, deprecated
#define FILM_G 'G' ///< Film G, BWY, deprecated
#define FILM_H 'H' ///< Film H, Freezer
#define FILM_J 'J' ///< Film J, BWR, "Spectra"
#define FILM_K 'K' ///< Film K, Wide temperature and embedded fast update
#define FILM_P 'P' ///< Film P, Embedded fast update
#define FILM_Q 'Q' ///< Film Q, BWRY, "Spectra 4"
#define FILM_T 'T' ///< Film T, proxy for P or K with touch

#define DRIVER_NONE 0x00 ///< Unknown
#define DRIVER_5 '5' ///< Driver 5
#define DRIVER_6 '6' ///< Driver 6
#define DRIVER_8 '8' ///< Driver 8
#define DRIVER_9 '9' ///< Driver 9
#define DRIVER_A 'A' ///< Driver A
#define DRIVER_B 'B' ///< Driver B
#define DRIVER_C 'C' ///< Driver C
#define DRIVER_D 'D' ///< Driver D
#define DRIVER_E 'E' ///< Driver E
#define DRIVER_F 'F' ///< Driver F
#define DRIVER_G 'G' ///< Driver G
#define DRIVER_H 'H' ///< Driver H
#define DRIVER_J 'J' ///< Driver J

#define LEVEL_CRITICAL 0x0001 ///< `*` Non-handled error, stop or exit
#define LEVEL_INFO 0x0008 ///< `.` Information
#define LEVEL_DEBUG 0x0010 ///< `-` Debug
#define LEVEL_SYSTEM 0x0020 ///< `=` System

#define SCREEN(S, F, D) ((uint32_t)((S & 0x0fff) << 16 | (F & 0xff) << 8 | (D & 0xff)))
#define SCREEN_SIZE(X) ((uint16_t)((X >> 16) & 0x0fff)) ///< Get size
#define SCREEN_FILM(X) ((uint8_t)((X >> 8) & 0xff)) ///< Get family
#define SCREEN_DRIVER(X) ((uint8_t)(X & 0xff)) ///< Get type
#define SCREEN_EXTRA(X) ((uint8_t)((X >> 28) & 0x0f)) ///< Get extra

#define eScreen_EPD_150_KS_0J SCREEN(SIZE_150, FILM_K, DRIVER_J) ///< reference 150-KS-0J
#define eScreen_EPD_152_KS_0J SCREEN(SIZE_152, FILM_K, DRIVER_J) ///< reference 152-KS-0J
#define eScreen_EPD_154_KS_0C SCREEN(SIZE_154, FILM_K, DRIVER_C) ///< reference 154-KS-0C
#define eScreen_EPD_206_KS_0E SCREEN(SIZE_206, FILM_K, DRIVER_E) ///< reference 206-KS-0E
#define eScreen_EPD_213_KS_0E SCREEN(SIZE_213, FILM_K, DRIVER_E) ///< reference 213-KS-0E
#define eScreen_EPD_266_KS_0C SCREEN(SIZE_266, FILM_K, DRIVER_C) ///< reference 266-KS-0C
#define eScreen_EPD_271_KS_09 SCREEN(SIZE_271, FILM_K, DRIVER_9) ///< reference 271-KS-09
#define eScreen_EPD_271_KS_0C SCREEN(SIZE_271, FILM_K, DRIVER_C) ///< reference 271-KS-0C
#define eScreen_EPD_290_KS_0F SCREEN(SIZE_290, FILM_K, DRIVER_F) ///< reference 290-KS-0F
#define eScreen_EPD_370_KS_0C SCREEN(SIZE_370, FILM_K, DRIVER_C) ///< reference 370-KS-0C
#define eScreen_EPD_417_KS_0D SCREEN(SIZE_417, FILM_K, DRIVER_D) ///< reference 417-KS-0D
#define eScreen_EPD_437_KS_0C SCREEN(SIZE_437, FILM_K, DRIVER_C) ///< reference 437-KS-0C

#define UPDATE_NONE 0x00 ///< No update
#define UPDATE_NORMAL 0x01 ///< Normal update, default
#define UPDATE_FAST 0x02 ///< Fast update
#define UPDATE_PARTIAL 0x03 ///< Partial update, deprecated

//#define hV_HAL_GPIO_define(X, Y) (pinMode(X, Y))
//#define hV_HAL_GPIO_set(X) (digitalWrite(X, HIGH))
#define setHigh(X) (digitalWrite(X, HIGH))
//#define hV_HAL_GPIO_clear(X) (digitalWrite(X, LOW))
#define setLow(X) (digitalWrite(X, LOW))
//#define hV_HAL_GPIO_get(X) (digitalRead(X))
//#define hV_HAL_GPIO_write(X, Y) (digitalWrite(X, Y))
//#define hV_HAL_GPIO_read(X) (digitalRead(X))

#define eScreen_EPD_t uint32_t
#define NOT_CONNECTED (uint8_t)0xff

#define SPI_FREQ 4000000

/**************************************************************************/
/*!
    @brief  Class for interfacing with SSD1619 EPD drivers
*/
/**************************************************************************/
class Pervasive_EPD : public Adafruit_EPD {
public:
    /*Pervasive_EPD(int width, int height, int16_t SID, int16_t SCLK, int16_t DC,
        int16_t RST, int16_t CS, int16_t SRCS, int16_t MISO,
        int16_t BUSY = -1);
    Pervasive_EPD(int width, int height, int16_t DC, int16_t RST, int16_t CS,
        int16_t SRCS, int16_t BUSY = -1, SPIClass* spi = &SPI);*/

    //Pervasive_EPD(eScreen_EPD_t eScreen_EPD, pins_t board, SPIClass* spi);
    Pervasive_EPD(eScreen_EPD_t eScreen_EPD,
        int16_t DC, int16_t RST,
        int16_t CS, int16_t SRCS, int16_t BUSY,
        SPIClass* spi);
    void begin(bool reset = true);
    void powerUp();
    void update(void);
    void powerDown();
    void setFastMode(bool fast);
protected:
    Pervasive_EPD(eScreen_EPD_t eScreen_EPD,
        int width, int height,
        int16_t DC, int16_t RST,
        int16_t CS, int16_t SRCS, int16_t BUSY,
        SPIClass* spi);

    eScreen_EPD_t u_eScreen_EPD;
    int8_t u_temperature; // = 25;
    bool u_flagOTP; // = false; // true = OTP read
    uint8_t COG_data[2]; // OTP
    bool s_flag50; // Register 0x50
    uint16_t b_delayCS = 50; // ms
    //uint8_t b_family = FAMILY_SMALL;
    uint8_t b_fsmPowerScreen = FSM_OFF;
    bool flagSPI = false; // Some SPI implementations require unique initialisation
    SPISettings _settingScreen;
    bool useFastUpdate;
    bool isFirstFrame = true;

    void beginSPI(uint32_t speed);
    uint8_t readSPI3();
    void writeSPI3(uint8_t value);
    void endSPI();

    void b_resume();
    void b_reset(uint32_t ms1, uint32_t ms2, uint32_t ms3, uint32_t ms4, uint32_t ms5);
    void b_sendIndexData(uint8_t index, const uint8_t* data, uint32_t size);
    void b_sendIndexFixed(uint8_t index, uint8_t data, uint32_t size);
    void b_sendCommand8(uint8_t command);
    void b_sendCommandData8(uint8_t command, uint8_t data);

    void COG_reset();
    void COG_getDataOTP();
    void COG_initial(uint8_t updateMode);
    void COG_update(uint8_t updateMode);
    void COG_stopDCDC();
    void COG_sendImageDataNormal(uint8_t* firstFrame, uint32_t sizeFrame); // First frame, blackBuffer
    void COG_sendImageDataFast(uint8_t* firstFrame, uint8_t* secondFrame, uint32_t sizeFrame); // First frame, blackBuffer

    void setBuffer2();
protected:
    uint8_t writeRAMCommand(uint8_t index);
    void setRAMAddress(uint16_t x, uint16_t y);
    void setRAMWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    void busy_wait(bool state);
    void busy_wait(void);
};

#endif
