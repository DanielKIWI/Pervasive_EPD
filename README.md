# Pervasive EPD Library
Subclass of [![Adafruit_EPD](https://github.com/adafruit/Adafruit_EPD)] and therefore subclass of [![Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library)].
Code adapted from [![Pervasive_Wide_Small](https://github.com/PervasiveDisplays/Pervasive_Wide_Small)] library.

This is a library for the Pervasive E-paper displays from 1.50" to 4.37":
  * https://www.pervasivedisplays.com/
  * https://mouser.com/manufacturer/pervasive-displays/

Using the Adafruit Eink friend as a driver board:
  * https://www.adafruit.com/product/4224

# Attention!!
Check the schematics of your Pervasive display and the Adafruit Eink friend to ensure they are compatible. I am not responsible for any damage that might result from the use of this library.
In my case the pins on the Pervasive display FFC (flexible flat cable) were in the wrong order when inserting into the Adafruit eink friends socket. So I had to use a FFC extension pcb and a ffc cable with the contacts on opposite sides to flip the order around.

# Adafruit Eink friend limitation and workaround
Pervasive Displays with Driver E, C or D require receiving data from the display, so they need the MOSI pin to be bidirectional, which the Adafruit Eink friend doesn't support out of the box:
The Eink friend sends the SPI signals through a LC245A Octal Bus transceiver to level shift it to 3.3v no matter what logic level your microcontroller uses. But this IC only supports doesn't support simultaneous bidirectional data transfer, which we would need to simultaneously send SPI clock signal and receive data over the MOSI line.
To solve this issue you can bypass the IC by soldering a thin wire from the SCK pin to the right FFC socket pin, and scratching away the existing pcb lines from the level shift IC to that FFC socket pin (See Image).

Written by Daniel Ivert
MIT license, all text above must be included in any redistribution
