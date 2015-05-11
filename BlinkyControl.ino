#include <Adafruit_NeoPixel.h>

#define  LED_PIN    12
#define  LED_COUNT  ( CUP_POSITIONS * 2 ) + 2

#define  LED_ON    4
#define  LED_OFF   0

#define TOTAL_COLORS  7
byte colorPalette [ TOTAL_COLORS ] [ 3 ] = { 
  { 255,   0,   0 },
  {   0, 255,   0 },
  {   0,   0, 255 },
  { 255, 255, 255 },
  { 255, 255,   0 },
  {   0, 255, 255 },
  {   0,   0,   0 }
};

#define  COLOR_OFF             6
#define  COLOR_DISPENSER_MOVE  5
#define  COLOR_PUMP_RUN        4
#define  COLOR_DRINK_FINISHED  3
#define  COLOR_MIXING_DRINK    2
#define  COLOR_CUP_WAITING     1
#define  COLOR_ORDER_WAITING   0

Adafruit_NeoPixel rgbLeds = Adafruit_NeoPixel ( LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800 );

void setupBlinkyController() {
  
  rgbLeds.begin();
  rgbLeds.setBrightness ( LED_ON );
}

void setColor ( int blinkyId, int colorIndex ) {
  rgbLeds.setPixelColor ( blinkyId, colorPalette [ colorIndex ] [ 0 ], colorPalette [ colorIndex ] [ 1 ], colorPalette [ colorIndex ] [ 2 ] );
}

void showPumpStatus ( bool pumpStatus ) {
  if ( pumpStatus ) {
    setColor ( 0, COLOR_PUMP_RUN );
  }
  else {
    setColor ( 0, COLOR_OFF );
  }
  rgbLeds.show ();
}

#define ORDER_LED_START  2
void showOrderQueue ( int startingOffset, int count ) {
  
  for ( int i = 0; i < CUP_POSITIONS; i ++ ) {
    setColor ( ORDER_LED_START + i, COLOR_OFF );
  }
  
  for ( int i = 0; i < count; i ++ ) {
    if ( i >= startingOffset ) {
      setColor ( ORDER_LED_START + i, COLOR_ORDER_WAITING );
    }
  }
  rgbLeds.show ();
}
