#include <Adafruit_NeoPixel.h>
//#include <avr/power.h>

#define  LED_PIN    12
#define  LED_COUNT  ( CUP_POSITIONS * 2 )

#define  EMPTY_COLOR  Color ( 150, 0, 0 )

Adafruit_NeoPixel rgbLeds = Adafruit_NeoPixel ( LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800 );

void setupCupController() {
  rgbLeds.begin();
}

void markCupEmpty ( int cupID ) {
  rgbLeds.setPixelColor ( cupID, rgbLeds.EMPTY_COLOR );
  rgbLeds.show();
}
