//#define DEBUG
#include "Pervasive_EPD.h"
#include "Adafruit_EPD.h"

#define EPD_RAM_BW 0x10
#define EPD_RAM_RED 0x13

#define BUSY_WAIT 500

// clang-format on

/**************************************************************************/
/*!
	@brief constructor if using external SRAM chip and software SPI
	@param width the width of the display in pixels
	@param height the height of the display in pixels
	@param SID the SID pin to use
	@param SCLK the SCLK pin to use
	@param DC the data/command pin to use
	@param RST the reset pin to use
	@param CS the chip select pin to use
	@param SRCS the SRAM chip select pin to use
	@param MISO the MISO pin to use
	@param BUSY the busy pin to use
*/
/**************************************************************************/
//Pervasive_EPD::Pervasive_EPD(int width, int height, int16_t SID,
//    int16_t SCLK, int16_t DC, int16_t RST,
//    int16_t CS, int16_t SRCS, int16_t MISO,
//    int16_t BUSY)
//    : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY) {
//    if ((height % 8) != 0) {
//        height += 8 - (height % 8);
//    }
//
//    buffer1_size = width * height / 8;
//    buffer2_size = buffer1_size;
//
//    if (SRCS >= 0) {
//        use_sram = true;
//        buffer1_addr = 0;
//        buffer2_addr = buffer1_size;
//        buffer1 = buffer2 = NULL;
//    }
//    else {
//        buffer1 = (uint8_t*)malloc(buffer1_size);
//        buffer2 = (uint8_t*)malloc(buffer2_size);
//    }
//
//    singleByteTxns = true;
//}
//
//// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
//
///**************************************************************************/
///*!
//    @brief constructor if using on-chip RAM and hardware SPI
//    @param width the width of the display in pixels
//    @param height the height of the display in pixels
//    @param DC the data/command pin to use
//    @param RST the reset pin to use
//    @param CS the chip select pin to use
//    @param SRCS the SRAM chip select pin to use
//    @param BUSY the busy pin to use
//*/
///**************************************************************************/
//Pervasive_EPD::Pervasive_EPD(int width, int height, int16_t DC,
//    int16_t RST, int16_t CS, int16_t SRCS,
//    int16_t BUSY, SPIClass* spi)
//    : Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY, spi) {
//    if ((height % 8) != 0) {
//        height += 8 - (height % 8);
//    }
//
//    buffer1_size = width * height / 8;
//    buffer2_size = buffer1_size;
//
//    if (SRCS >= 0) {
//        use_sram = true;
//        buffer1_addr = 0;
//        buffer2_addr = buffer1_size;
//        buffer1 = buffer2 = NULL;
//    }
//    else {
//        buffer1 = (uint8_t*)malloc(buffer1_size);
//        buffer2 = (uint8_t*)malloc(buffer2_size);
//    }
//
//    singleByteTxns = true;
//}

void log(uint16_t level, const char* format, ...)
{
	char buffer[128] = { 0x00 };
	// memset(&buffer, 0x00, sizeof(buffer));

	// Content
	char* frame = &buffer[strlen(buffer)];
	va_list args;
	va_start(args, format);
	vsnprintf(frame, 127, format, args);
	va_end(args);

	Serial.print("hV _ ");
	Serial.println(buffer);
}

void exit(uint8_t code)
{
	log(LEVEL_INFO, "Exit with code %i", code);
	Serial.println();
	while (true)
	{
		delay(1000);
	}
}

int GeHeight(eScreen_EPD_t eScreen_EPD) {
	switch (eScreen_EPD)
	{
	case eScreen_EPD_150_KS_0J:
	case eScreen_EPD_152_KS_0J:
		return 200;
	case eScreen_EPD_154_KS_0C:
		return 152;
	case eScreen_EPD_206_KS_0E:
		return 248;
	case eScreen_EPD_213_KS_0E:
		return 212;
	case eScreen_EPD_266_KS_0C:
		return 296;
	case eScreen_EPD_271_KS_09:
	case eScreen_EPD_271_KS_0C:
		return 264;
	case eScreen_EPD_290_KS_0F:
		return 384;
	case eScreen_EPD_370_KS_0C:
		return 416;
	case eScreen_EPD_417_KS_0D:
		return 400;
	case eScreen_EPD_437_KS_0C:
		return 480;

	default:
		break;
	}
}
int GetWidth(eScreen_EPD_t eScreen_EPD) {
	switch (eScreen_EPD)
	{
	case eScreen_EPD_150_KS_0J:
	case eScreen_EPD_152_KS_0J:
		return 200;
	case eScreen_EPD_154_KS_0C:
		return 152;
	case eScreen_EPD_206_KS_0E:
		return 128;
	case eScreen_EPD_213_KS_0E:
		return 104;
	case eScreen_EPD_266_KS_0C:
		return 152;
	case eScreen_EPD_271_KS_09:
	case eScreen_EPD_271_KS_0C:
		return 176;
	case eScreen_EPD_290_KS_0F:
		return 168;
	case eScreen_EPD_370_KS_0C:
		return 240;
	case eScreen_EPD_417_KS_0D:
		return 300;
	case eScreen_EPD_437_KS_0C:
		return 176;

	default:
		break;
	}
}

void Pervasive_EPD::beginSPI(uint32_t speed)
{
	if (flagSPI != true)
	{
		_settingScreen = { speed, MSBFIRST, SPI_MODE0 };
		SPI.begin();
		SPI.beginTransaction(_settingScreen);
		flagSPI = true;
	}
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

void Pervasive_EPD::endSPI()
{
	if (flagSPI != false)
	{
		SPI.end();
		flagSPI = false;
	}
}

void Pervasive_EPD::b_resume()
{
	// Target FSM_ON
	// Source FSM_SLEEP -> FSM_SLEEP
	//        FSM_OFF   -> FSM_SLEEP

	if ((b_fsmPowerScreen & FSM_GPIO_MASK) != FSM_GPIO_MASK)
	{
		// Optional power circuit
		//if (b_pin.panelPower != NOT_CONNECTED) // generic
		//{
		//    pinMode(b_pin.panelPower, OUTPUT);
		//    setHigh(b_pin.panelPower);
		//}

		// Configure GPIOs
		pinMode(_busy_pin, INPUT);

		pinMode(_dc_pin, OUTPUT);
		setHigh(_dc_pin);

		pinMode(_reset_pin, OUTPUT);
		setHigh(_reset_pin);

		pinMode(_cs_pin, OUTPUT);
		setHigh(_cs_pin); // CS# = 1

		//if (_css_pin != NOT_CONNECTED) // generic
		//{
		//    pinMode(_css_pin, OUTPUT);
		//    setHigh(_css_pin);
		//}

		// External SPI memory
		//if (b_pin.flashCS != NOT_CONNECTED) // generic
		//{
		//    pinMode(b_pin.flashCS, OUTPUT);
		//    setHigh(b_pin.flashCS);
		//}
		//if (b_pin.flashCSS != NOT_CONNECTED) // generic
		//{
		//    pinMode(b_pin.flashCSS, OUTPUT);
		//    setHigh(b_pin.flashCSS);
		//}

		// External SD card
		//if (b_pin.cardCS != NOT_CONNECTED) // generic
		//{
		//    pinMode(b_pin.cardCS, OUTPUT);
		//    setHigh(b_pin.cardCS);
		//}
		//if (b_pin.cardDetect != NOT_CONNECTED) // generic
		//{
		//    pinMode(b_pin.cardCS, INPUT);
		//}
		b_fsmPowerScreen |= FSM_GPIO_MASK;
	}
}

//void Pervasive_EPD::hardwareReset(void) {
	//b_reset(1, 5, 5, 10, 20);
//}

void Pervasive_EPD::b_reset(uint32_t ms1, uint32_t ms2, uint32_t ms3, uint32_t ms4, uint32_t ms5)
{
	delay(ms1); // Wait for power stabilisation
	setHigh(_reset_pin); // RESET = HIGH
	delay(ms2);
	setLow(_reset_pin); // RESET = LOW
	delay(ms3);
	setHigh(_reset_pin); // RESET = HIGH
	delay(ms4);
	setHigh(_cs_pin); // CS = HIGH, unselect
	delay(ms5);
}


void Pervasive_EPD::b_sendIndexData(uint8_t index, const uint8_t* data, uint32_t size)
{
	// Command mode
	setLow(_dc_pin); // DC Low = Command

	// Select
	setLow(_cs_pin); // CS Low = Select
	//if (b_family == FAMILY_LARGE) // panelCSS already checked
	//{
	//    setLow(_cs_pinS);
	//    delayMicroseconds(450); // 450 + 50 = 500
	//}
	delayMicroseconds(b_delayCS);

	// Send command
	SPI.transfer(index);
	delayMicroseconds(b_delayCS);

	// Data mode
	setHigh(_dc_pin); // DC High = Data

	delayMicroseconds(b_delayCS);

	// Send data
	for (uint32_t i = 0; i < size; i++)
	{
		SPI.transfer(data[i]);
	}

	// Unselect
	delayMicroseconds(b_delayCS);
	setHigh(_cs_pin); // CS High
	//if (b_family == FAMILY_LARGE) // panelCSS already checked
	//{
	//    delayMicroseconds(450); // 450 + 50 = 500
	//    setHigh(_cs_pinS);
	//}
	delayMicroseconds(b_delayCS);
}

void Pervasive_EPD::b_sendIndexFixed(uint8_t index, uint8_t data, uint32_t size)
{
	setLow(_dc_pin); // DC Low = Command
	setLow(_cs_pin); // CS Low = Select

	delayMicroseconds(b_delayCS);
	SPI.transfer(index);
	delayMicroseconds(b_delayCS);

	setHigh(_dc_pin); // DC High = Data

	delayMicroseconds(b_delayCS);
	for (uint32_t i = 0; i < size; i++)
	{
		SPI.transfer(data); // b_sendIndexFixed
	}
	delayMicroseconds(b_delayCS);

	setHigh(_cs_pin); // CS High = Unselect
}


void Pervasive_EPD::b_sendCommand8(uint8_t command) {
	//return Adafruit_EPD::b_sendCommand8(command, end);

	setLow(_dc_pin);
	setLow(_cs_pin);

	SPI.transfer(command);

	setHigh(_cs_pin);
}

void Pervasive_EPD::b_sendCommandData8(uint8_t command, uint8_t data) {

	setLow(_dc_pin);
	setLow(_cs_pin);

	SPI.transfer(command);

	setHigh(_dc_pin); // HIGH = data
	SPI.transfer(data);

	setHigh(_cs_pin);
	//b_sendCommand8(command, false);
	//b_sendCommand8(data);
}

void Pervasive_EPD::COG_reset()
{
	// Application note � 2. Power on COG driver
	b_reset(1, 5, 5, 10, 20);

	// Check after reset
	switch (u_eScreen_EPD)
	{
	case eScreen_EPD_150_KS_0J:
	case eScreen_EPD_152_KS_0J:

		if (digitalRead(_busy_pin) == HIGH)
		{
			Serial.println();
			log(LEVEL_CRITICAL, "Incorrect type for 1.52-Wide");
			exit(0x01);
		}
		break;

	default:

		break;
	}
}

void Pervasive_EPD::COG_getDataOTP()
{
	endSPI(); // With unicity check

	// Read OTP
	u_flagOTP = false;

	// Application note � 3. Read OTP memory
	// Register 0x50 flag
	// Additional settings for fast update, 154 206 213 266 271A 370 and 437 screens (s_flag50)
	switch (u_eScreen_EPD)
	{
	case eScreen_EPD_154_KS_0C:
	case eScreen_EPD_206_KS_0E:
	case eScreen_EPD_213_KS_0E:
	case eScreen_EPD_266_KS_0C:
	case eScreen_EPD_271_KS_0C: // 2.71(A)
	case eScreen_EPD_370_KS_0C:
	case eScreen_EPD_437_KS_0C:

		s_flag50 = true;
		break;

	default:

		s_flag50 = false;
		break;
	}

	// Screens with no OTP
	switch (u_eScreen_EPD)
	{
	case eScreen_EPD_150_KS_0J:
	case eScreen_EPD_152_KS_0J:
	case eScreen_EPD_290_KS_0F:

		u_flagOTP = true;
		Serial.println("hV . OTP check passed - embedded PSR");
		return; // No PSR
		break;

	default:

		break;
	}

	setHigh(_dc_pin);
	setHigh(_reset_pin);
	setHigh(_cs_pin);

	b_reset(0, 5, 5, 10, 20);
	uint8_t ui8 = 0;
	uint16_t offsetA5 = 0x0000;
	uint16_t offsetPSR = 0x0000;
	uint16_t u_readBytes = 2;

	setLow(_dc_pin); // Command
	setLow(_cs_pin); // Select
	writeSPI3(0xa2);
	setHigh(_cs_pin); // Unselect
	delay(10);

	setHigh(_dc_pin); // Data
	setLow(_cs_pin); // Select
	ui8 = readSPI3(); // Dummy
	setHigh(_cs_pin); // Unselect

	log(LEVEL_CRITICAL, "1) first: 0x%02x", ui8);

	setLow(_cs_pin); // Select
	ui8 = readSPI3(); // First byte to be checked
	setHigh(_cs_pin); // Unselect

	log(LEVEL_CRITICAL, "2) first: 0x%02x", ui8);
	// Check bank
	uint8_t bank = ((ui8 == 0xa5) ? 0 : 1);

	switch (u_eScreen_EPD)
	{
	case eScreen_EPD_271_KS_09:

		offsetPSR = 0x004b;
		offsetA5 = 0x0000;

		if (bank > 0)
		{
			COG_data[0] = 0xcf;
			COG_data[1] = 0x82;
			return;
		}
		break;

	case eScreen_EPD_154_KS_0C:
	case eScreen_EPD_266_KS_0C:
	case eScreen_EPD_271_KS_0C: // 2.71(A)
	case eScreen_EPD_370_KS_0C:
	case eScreen_EPD_437_KS_0C:

		offsetPSR = (bank == 0) ? 0x0fb4 : 0x1fb4;
		offsetA5 = (bank == 0) ? 0x0000 : 0x1000;
		break;

	case eScreen_EPD_206_KS_0E:
	case eScreen_EPD_213_KS_0E:

		offsetPSR = (bank == 0) ? 0x0b1b : 0x171b;
		offsetA5 = (bank == 0) ? 0x0000 : 0x0c00;
		break;

	case eScreen_EPD_417_KS_0D:

		// COG_data[0] = 0xcf;
		// COG_data[1] = 0x82;
		// return;


		offsetPSR = (bank == 0) ? 0x0b1f : 0x171f;
		offsetA5 = (bank == 0) ? 0x0000 : 0x0c00;
		break;

	default:
		Serial.println();
		log(LEVEL_CRITICAL, "OTP check failed - Screen %i-%cS-0%c not supported", SCREEN_SIZE(u_eScreen_EPD), SCREEN_FILM(u_eScreen_EPD), SCREEN_DRIVER(u_eScreen_EPD));
		exit(0x01);
		break;
	}

	// Check second bank
	if (offsetA5 > 0x0000)
	{
		for (uint16_t index = 1; index < offsetA5; index += 1)
		{
			setLow(_cs_pin); // CS low = Select
			ui8 = readSPI3();
			//log(LEVEL_INFO, "A) %i: 0x%02x", index, ui8);
			setHigh(_cs_pin); // CS high = Unselect
		}

		setLow(_cs_pin); // CS low = Select
		ui8 = readSPI3(); // First byte to be checked
		setHigh(_cs_pin); // CS high = Unselect

		//log(LEVEL_CRITICAL, "3) first: 0x%02x", ui8);
		if (ui8 != 0xa5)
		{
			Serial.println();
			log(LEVEL_CRITICAL, "OTP check failed - Bank %i, first 0x%02x, expected 0x%02x", bank, ui8, 0xa5);
			exit(0x01);
		}
	}

	switch (u_eScreen_EPD)
	{
	case eScreen_EPD_271_KS_09:

		log(LEVEL_INFO, "OTP check passed - Bank %i, first 0x%02x %s", bank, ui8, (bank == 0) ? "as expected" : "not checked");
		break;

	default:

		log(LEVEL_INFO, "OTP check passed - Bank %i, first 0x%02x as expected", bank, ui8);
		break;
	}

	for (uint16_t index = offsetA5 + 1; index < offsetPSR; index += 1)
	{
		setLow(_cs_pin); // Select
		ui8 = readSPI3();
		//log(LEVEL_INFO, "B) %i: 0x%02x", index, ui8);
		setHigh(_cs_pin); // Unselect
	}
	// Populate COG_initialData
	for (uint16_t index = 0; index < u_readBytes; index += 1)
	{
		setLow(_cs_pin); // Select
		COG_data[index] = readSPI3(); // Read OTP
		//log(LEVEL_INFO, "C) %i: 0x%02x", index, COG_data[index]);
		setHigh(_cs_pin); // Unselect
	}

	u_flagOTP = true;

//#if (DEBUG_OTP == 1) // Debug COG_data
//	debugOTP(COG_data, u_readBytes, COG_WIDE_SMALL, SCREEN_DRIVER(u_eScreen_EPD));
//#endif // DEBUG_OTP
}


void Pervasive_EPD::COG_initial(uint8_t updateMode)
{
	// Application note § 4. Input initial command
	switch (u_eScreen_EPD)
	{
	case eScreen_EPD_150_KS_0J:
	case eScreen_EPD_152_KS_0J:

		// Soft reset
		b_sendCommand8(0x12);
		setLow(_dc_pin);
		busy_wait(LOW); // 150 and 152 specific

		// Work settings
		b_sendCommandData8(0x1a, u_temperature);

		if (updateMode == UPDATE_NORMAL)
		{
			b_sendCommandData8(0x22, 0xd7);
		}
		else if (updateMode == UPDATE_FAST)
		{
			b_sendCommandData8(0x3c, 0xc0);
			b_sendCommandData8(0x22, 0xdf);
		}
		break;

	default:

		// Work settings
		uint8_t indexTemperature; // Temperature
		uint8_t index00_work[2]; // PSR

		// FILM_K already checked
		if (updateMode != UPDATE_NORMAL) // Specific settings for fast update
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
		b_sendCommandData8(0x00, 0x0e); // Soft-reset
		busy_wait();

		b_sendCommandData8(0xe5, indexTemperature); // Input Temperature
		b_sendCommandData8(0xe0, 0x02); // Activate Temperature
		switch (u_eScreen_EPD)
		{
		case eScreen_EPD_290_KS_0F: // No PSR

			b_sendCommandData8(0x4d, 0x55);
			b_sendCommandData8(0xe9, 0x02);
			break;

		default:

			b_sendIndexData(0x00, index00_work, 2); // PSR
			break;
		}


		// Specific settings for fast update, all screens
		// FILM_K already checked
		if (updateMode != UPDATE_NORMAL)
		{
			b_sendCommandData8(0x50, 0x07); // Vcom and data interval setting
		}
		break;
	}
}


void Pervasive_EPD::COG_sendImageDataNormal(uint8_t* firstFrame, uint32_t sizeFrame) // First frame, blackBuffer
{
	// firstFrame: New Image for NORMAL, Old Image for FAST
	// secondFrame: 0x00 data for NORMAL, New Image for FAST

	// Application note § 4. Input image to the EPD
	// b_sendIndexData(0x10, s_newImage, sizeFrame); // First frame, blackBuffer
	// b_sendIndexData(0x10, frame, sizeFrame); // First frame, blackBuffer

	// Send image data
	switch (u_eScreen_EPD)
	{
	case eScreen_EPD_150_KS_0J:
	case eScreen_EPD_152_KS_0J:

		b_sendIndexData(0x24, firstFrame, sizeFrame); // Next frame, blackBuffer
		b_sendIndexFixed(0x26, 0x00, sizeFrame); // Previous frame, 0x00
		break;

	default:

		b_sendIndexData(0x10, firstFrame, sizeFrame); // First frame, blackBuffer
		b_sendIndexFixed(0x13, 0x00, sizeFrame); // Second frame, 0x00
		break;
	} // u_eScreen_EPD
}

void Pervasive_EPD::COG_sendImageDataFast(uint8_t* firstFrame, uint8_t* secondFrame, uint32_t sizeFrame) // First frame, blackBuffer
{
	// firstFrame: New Image for NORMAL, Old Image for FAST
	// secondFrame: 0x00 data for NORMAL, New Image for FAST

	// Application note § 4. Input image to the EPD
	// b_sendIndexData(0x10, s_newImage, sizeFrame); // First frame, blackBuffer
	// b_sendIndexData(0x10, frame, sizeFrame); // First frame, blackBuffer

	// Send image data
	switch (u_eScreen_EPD)
	{
	case eScreen_EPD_150_KS_0J:
	case eScreen_EPD_152_KS_0J:

		b_sendIndexData(0x24, secondFrame, sizeFrame); // Next frame, blackBuffer
		b_sendIndexData(0x26, firstFrame, sizeFrame); // Previous frame, 0x00
		break;

	default:
		// Additional settings for fast update, 154 213 266 370 and 437 screens (s_flag50)
		if (s_flag50)
		{
			b_sendCommandData8(0x50, 0x27); // Vcom and data interval setting
		}

		b_sendIndexData(0x10, secondFrame, sizeFrame); // First frame, blackBuffer
		b_sendIndexData(0x13, firstFrame, sizeFrame); // Second frame, 0x00

		// Additional settings for fast update, 154 213 266 370 and 437 screens (s_flag50)
		if (s_flag50)
		{
			b_sendCommandData8(0x50, 0x07); // Vcom and data interval setting
		}
		break;
	} // u_eScreen_EPD
}

void Pervasive_EPD::COG_update(uint8_t updateMode)
{
	// Application note § 6. Send updating command
	switch (u_eScreen_EPD)
	{
	case eScreen_EPD_150_KS_0J:
	case eScreen_EPD_152_KS_0J:

		busy_wait(LOW); // 152 specific
		b_sendCommand8(0x20); // Display Refresh
		setHigh(_cs_pin); // CS# = 1
		busy_wait(LOW); // 152 specific
		break;

	default:

		busy_wait();

		b_sendCommand8(0x04); // Power on
		busy_wait();

		b_sendCommand8(0x12); // Display Refresh
		busy_wait();
		break;
	}
}

void Pervasive_EPD::COG_stopDCDC()
{
	// Application note § 7. Turn-off DC/DC
	switch (u_eScreen_EPD)
	{
	case eScreen_EPD_150_KS_0J:
	case eScreen_EPD_152_KS_0J:

		break;

	default:

		b_sendCommand8(0x02); // Turn off DC/DC
		busy_wait();
		break;
	}
}

/**************************************************************************/
/*!
	@brief wait for busy signal to end
*/
/**************************************************************************/
void Pervasive_EPD::busy_wait(bool state) {
	const uint8_t delayMS = 32;
#ifdef DEBUG
	Serial.printf("busy_wait, pin %i", _busy_pin);
	uint32_t i = 0;
#endif
	while (digitalRead(_busy_pin) != state)
	{
		delay(delayMS); // non-blocking
#ifdef DEBUG
		i++;
		if (i % (1000 / delayMS) == 0) {
			Serial.print(".");
		}
#endif
	}
#ifdef DEBUG
	Serial.printf(" %i ms\n", i * delayMS);
#endif
}
void Pervasive_EPD::busy_wait(void) {
	busy_wait(HIGH);
}

//void Pervasive_EPD::busy_wait(void) {
//    b_waitBusy(LOW);
//    if (_busy_pin != NOT_CONNECTED) {
//        Serial.printf("busy_wait, pin %i", _busy_pin);
//        uint32_t i = 0;
//        while (digitalRead(_busy_pin) == HIGH) { // wait for busy low
//            delay(10);
//            i++;
//            if (i % 100 == 0) {
//                Serial.print(".");
//            }
//        }
//        Serial.printf(" %i s\n", _busy_pin, (i / 100));
//    }
//    else {
//        delay(BUSY_WAIT);
//    }
//}



//Pervasive_EPD::Pervasive_EPD(eScreen_EPD_t eScreen_EPD, pins_t board, SPIClass* spi)
Pervasive_EPD::Pervasive_EPD(eScreen_EPD_t eScreen_EPD,
	int width, int height,
	int16_t DC, int16_t RST,
	int16_t CS, int16_t SRCS, int16_t BUSY,
	SPIClass* spi)
	: Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY, spi) {
	u_eScreen_EPD = eScreen_EPD;

	//b_pin = board;

	//use_sram = SRCS >= 0;
	use_sram = false;

	if ((height % 8) != 0) {
		height += 8 - (height % 8);
	}

	buffer1_size = width * height / 8;
	buffer2_size = buffer1_size;
	Serial.printf("buffer1_size: %i\n", buffer1_size);
	if (use_sram) {
		buffer1_addr = 0;
		buffer2_addr = buffer1_size;
		buffer1 = buffer2 = NULL;
	}
	else {
		buffer1 = (uint8_t*)malloc(buffer1_size);
		buffer2 = (uint8_t*)malloc(buffer2_size);
	}

	singleByteTxns = true;
}

Pervasive_EPD::Pervasive_EPD(eScreen_EPD_t eScreen_EPD,
	int16_t DC, int16_t RST,
	int16_t CS, int16_t SRCS, int16_t BUSY,
	SPIClass* spi)
	: Pervasive_EPD(eScreen_EPD, GetWidth(eScreen_EPD), GeHeight(eScreen_EPD), DC, RST, CS, SRCS, BUSY, spi) {
}

/**************************************************************************/
/*!
	@brief begin communication with and set up the display.
	@param reset if true the reset pin will be toggled.
*/
/**************************************************************************/
void Pervasive_EPD::begin(bool reset) {
	//Adafruit_EPD::begin(reset);
	use_sram = false;

	setBlackBuffer(0, true);  // black defaults to inverted
	setColorBuffer(0, true);  // no color
	//setColorBuffer(1, false); // red defaults to un inverted


	/*layer_colors[EPD_WHITE] = 0b00;
	layer_colors[EPD_BLACK] = 0b01;
	layer_colors[EPD_RED] = 0b10;
	layer_colors[EPD_GRAY] = 0b10;
	layer_colors[EPD_DARK] = 0b01;
	layer_colors[EPD_LIGHT] = 0b10;*/

	layer_colors[EPD_WHITE] = 0b01;
	layer_colors[EPD_BLACK] = 0b00;
	layer_colors[EPD_RED] = 0b10;
	layer_colors[EPD_GRAY] = 0b10;
	layer_colors[EPD_DARK] = 0b10;
	layer_colors[EPD_LIGHT] = 0b01;

	if (use_sram) {
		sram.begin();
		sram.write8(0, K640_SEQUENTIAL_MODE, MCPSRAM_WRSR);
	}

	// Serial.println("set pins");
	// set pin directions
	pinMode(_dc_pin, OUTPUT);
	pinMode(_cs_pin, OUTPUT);

#if defined(BUSIO_USE_FAST_PINIO)
	csPort = (BusIO_PortReg*)portOutputRegister(digitalPinToPort(_cs_pin));
	csPinMask = digitalPinToBitMask(_cs_pin);
	dcPort = (BusIO_PortReg*)portOutputRegister(digitalPinToPort(_dc_pin));
	dcPinMask = digitalPinToBitMask(_dc_pin);
#endif

	csHigh();

	//if (!spi_dev->begin()) {
		//return;
	//}

	// Serial.println("hard reset");
	//if (reset) {
		//hardwareReset();
	//}

	// Serial.println("busy");
	if (_busy_pin >= 0) {
		pinMode(_busy_pin, INPUT);
	}


	log(LEVEL_INFO, "Begin");
	// Pervasive init
	b_fsmPowerScreen = FSM_OFF;

	b_resume(); // GPIO

	COG_reset(); // Reset
	COG_getDataOTP(); // 3-wire SPI read OTP memory
	//

	//powerDown();
	Serial.printf("COG_data: 0x%02x, 0x%02x", COG_data[0], COG_data[1]);
}

/**************************************************************************/
/*!
	@brief signal the display to update
*/
/**************************************************************************/
void Pervasive_EPD::update() {
	//TODO

	b_resume(); // GPIO
	COG_reset(); // Reset

	if (u_flagOTP == false)
	{
		COG_getDataOTP(); // 3-wire SPI read OTP memory
		COG_reset(); // Reset
	}

	// Start SPI
	beginSPI(SPI_FREQ); // Fast 16 MHz, with unicity check

	if (useFastUpdate && !isFirstFrame) {
		COG_initial(UPDATE_FAST); // Initialise
		COG_sendImageDataFast(buffer1, buffer2, buffer1_size);

		COG_update(UPDATE_FAST); // Update
	}
	else {
		COG_initial(UPDATE_NORMAL); // Initialise
		COG_sendImageDataNormal(buffer1, buffer1_size);

		COG_update(UPDATE_NORMAL); // Update
	}
	setBuffer2();

	COG_stopDCDC(); // Power off

}

void Pervasive_EPD::setFastMode(bool fast) {
	useFastUpdate = fast;
	if (fast)
		setBuffer2();
}

void Pervasive_EPD::setBuffer2() {
	isFirstFrame = false;
	for (int i = 0; i < buffer1_size; i++) {
		buffer2[i] = buffer1[i];
	}
}

/**************************************************************************/
/*!
	@brief start up the display
*/
/**************************************************************************/
void Pervasive_EPD::powerUp() {

	b_resume();
	COG_reset();

	if (u_flagOTP == false)
	{
		COG_getDataOTP(); // 3-wire SPI read OTP memory
		COG_reset(); // Reset
	}
}

/**************************************************************************/
/*!
	@brief wind down the display
*/
/**************************************************************************/
void Pervasive_EPD::powerDown() {
	//uint8_t buf[1];
	//// Only deep sleep if we can get out of it
	//if (_reset_pin >= 0) {
	//    // deep sleep
	//    buf[0] = 0x01;
	//    b_sendCommand8(SSD1619_DEEP_SLEEP, buf, 1);
	//    delay(100);
	//}
	//else {
	//    b_sendCommand8(SSD1619_SW_RESET);
	//    busy_wait();
	//}
}

/**************************************************************************/
/*!
	@brief Send the specific command to start writing to EPD display RAM
	@param index The index for which buffer to write (0 or 1 or tri-color
   displays) Ignored for monochrome displays.
	@returns The byte that is read from SPI at the same time as sending the
   command
*/
/**************************************************************************/
uint8_t Pervasive_EPD::writeRAMCommand(uint8_t index) {
	/*if (index == 0) {
		return b_sendCommand8(SSD1619_WRITE_RAM1, false);
	}
	if (index == 1) {
		return b_sendCommand8(SSD1619_WRITE_RAM2, false);
	}
	return 0;*/
}

/**************************************************************************/
/*!
	@brief Some displays require setting the RAM address pointer
	@param x X address counter value
	@param y Y address counter value
*/
/**************************************************************************/
void Pervasive_EPD::setRAMAddress(uint16_t x, uint16_t y) {
	//(void)x;
	//(void)y;

	//uint8_t buf[2];

	//// set RAM x address count
	//buf[0] = 0x00;
	//b_sendCommand8(SSD1619_SET_RAMXCOUNT, buf, 1);

	//// set RAM y address count
	//buf[0] = 0x0;
	//buf[1] = 0x0;
	//b_sendCommand8(SSD1619_SET_RAMYCOUNT, buf, 2);
}

/**************************************************************************/
/*!
	@brief Some displays require setting the RAM address pointer
	@param x X address counter value
	@param y Y address counter value
*/
/**************************************************************************/
void Pervasive_EPD::setRAMWindow(uint16_t x1, uint16_t y1, uint16_t x2,
	uint16_t y2) {
	//uint8_t buf[5];

	//// Set ram X start/end postion
	//buf[0] = x1 / 8;
	//buf[1] = x2 / 8;
	//b_sendCommand8(SSD1619_SET_RAMXPOS, buf, 2);

	//// Set ram Y start/end postion
	//buf[0] = y1;
	//buf[1] = y1 >> 8;
	//buf[2] = y2;
	//buf[3] = y2 >> 8;
	//b_sendCommand8(SSD1619_SET_RAMYPOS, buf, 4);
}
