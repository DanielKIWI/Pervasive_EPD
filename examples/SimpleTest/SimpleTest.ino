// // DEMO Image Set <Comment out when in User Mode>
//Setting Size for demo images
// // Screen Size: 152, 154, 206, 213, 266, 271, 290, 370, 417, 437
#define SCRN 417
#include "globalupdate_src/demoImageData.h"
#include "fastupdate_src/demoImageData.h"

#include "Pervasive_EPD.h"
#include "SPI.h"

//Screen Options:
//eScreen_EPD_150_KS_0J
//eScreen_EPD_152_KS_0J
//eScreen_EPD_154_KS_0C
//eScreen_EPD_206_KS_0E
//eScreen_EPD_213_KS_0E
//eScreen_EPD_266_KS_0C
//eScreen_EPD_271_KS_09
//eScreen_EPD_271_KS_0C
//eScreen_EPD_290_KS_0F
//eScreen_EPD_370_KS_0C
//eScreen_EPD_417_KS_0D
//eScreen_EPD_437_KS_0C

Pervasive_EPD display(eScreen_EPD_417_KS_0D, 
    32, 33,
    4, 5, 25,
    &SPI);

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("setup");
    display.begin();
    display.setRotation(1);
    delay(1000);
    //Serial.println("updateNormal");
    //long t = millis();
    //display.updateNormal(BW_monoBuffer, frameSize);
    //Serial.println("updateNormal took " + String(millis() - t));
    //delay(10000);


    Serial.println("GFX test 1");
    display.fillScreen(EPD_WHITE);
    display.setCursor(100, 100);
    display.setTextColor(EPD_BLACK);
    display.setTextSize(2);
    display.print("Test");
    display.update();
    Serial.println("GFX test 1 done");
    delay(1000);

    Serial.println("GFX test 2");
    display.fillScreen(EPD_WHITE);
    display.drawBitmap(0, 0, BW_monoBuffer, 400, 300, EPD_BLACK, EPD_WHITE);
    //display.drawBitmap(0, 0, BW_monoBuffer, 300, 400, EPD_BLACK);
    display.update();
    Serial.println("GFX test 2 done");
    delay(1000);


    Serial.println("GFX Fast test 1");
    display.setFastMode(true);
    display.clearBuffer();// fillScreen(EPD_WHITE);
    display.drawBitmap(0, 0, FastPic_w, 400, 300, EPD_BLACK, EPD_WHITE);
    display.update();
    Serial.println("GFX Fast test 1 done");
    delay(1000);

    Serial.println("GFX Fast test 2");
    display.clearBuffer();
    display.drawBitmap(0, 0, FastPic_b, 400, 300, EPD_BLACK, EPD_WHITE);
    display.update();
    Serial.println("GFX Fast test 2 done");
    delay(1000);

    Serial.println("GFX Fast test 3");
    display.clearBuffer();
    display.drawBitmap(0, 0, FastPic_2, 400, 300, EPD_BLACK), EPD_WHITE;
    display.update();
    Serial.println("GFX Fast test 3 done");
    delay(1000);

    Serial.println("GFX Fast test 4");
    display.clearBuffer();
    display.drawBitmap(0, 0, FastPic_3, 400, 300, EPD_BLACK, EPD_WHITE);
    display.update();
    Serial.println("GFX Fast test 4 done");
    delay(1000);

    Serial.println("GFX Fast test 5");
    display.clearBuffer();
    display.drawBitmap(0, 0, FastPic_4, 400, 300, EPD_BLACK, EPD_WHITE);
    display.update();
    Serial.println("GFX Fast test 5 done");
    delay(1000);

    /*Serial.println("Fast Test 2");
    display.updateFast(FastPic_w, FastPic_1, frameSize);
    delay(10000);
    Serial.println("Fast Test 3");
    display.updateFast(FastPic_b, FastPic_w, frameSize);
    delay(1000);
    Serial.println("Fast Test 4");
    display.updateFast(FastPic_2, FastPic_b, frameSize);
    delay(1000);
    Serial.println("Fast Test 5");
    display.updateFast(FastPic_3, FastPic_2, frameSize);*/
    //delay(1000);
    //Serial.println("Fast Test 6");
    //display.updateFast(FastPic_4, FastPic_3, frameSize);
    // delay(1000);
    // Serial.println("Fast Test 7");
    // display.updateFast(FastPic_b, FastPic_4, frameSize);
    // delay(1000);
    // Serial.println("Fast Test 8");
    // display.updateFast(FastPic_w, FastPic_b, frameSize);
    // delay(1000);
    // Serial.println("Fast Test 9");
}

void loop()
{
    delay(1000);
}
