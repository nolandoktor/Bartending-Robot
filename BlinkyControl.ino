#include <Adafruit_NeoPixel.h>

#define  LED_PIN    10
#define  LED_COUNT  ( CUP_POSITIONS * 2 ) + 2

#define  LED_ON    4
#define  LED_OFF   0

#define TOTAL_COLORS  8
byte colorPalette [ TOTAL_COLORS ] [ 3 ] = { 
  { 255,   0,   0 },
  {   0, 255,   0 },
  {   0,   0, 255 },
  { 255, 255, 255 },
  { 255, 255,   0 },
  {   0, 255, 255 },
  {   0,   0,   0 },
  { 255,   0, 255 }
};

#define  COLOR_CUP_DETECTED    7
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
void showOrderQueue ( int startingOffset, int maxSize, int * orderQueStatus ) {
  
  #if defined( SOFTWARE_DEBUG )
    Serial.print ( "OrderQue Blinkies startingOffset: " );
    Serial.print ( startingOffset );
    Serial.print ( ", maxSize: " );
    Serial.print ( maxSize );
    Serial.print ( ", orderQueStatus [" );
    for ( int i = 0; i < maxSize; i ++ ) {
      Serial.print ( orderQueStatus [ i ] );
      Serial.print ( ", " );
    }
    Serial.println ( "]" );
  #endif

  for ( int i = 0; i < maxSize; i ++ ) {
    int index = ( i + startingOffset ) % maxSize;
    if ( orderQueStatus [ i ] != -1 ) {
      setColor ( ORDER_LED_START + index, COLOR_ORDER_WAITING );
    }
    else {
      setColor ( ORDER_LED_START + index, COLOR_OFF );
    }
  }

  rgbLeds.show ();
}

#define CUP_LED_START  ORDER_LED_START + CUP_POSITIONS
void showCupStatus ( int cupId, bool isPresent ) {
  if ( isPresent ) {
    setColor ( CUP_LED_START + cupId, COLOR_CUP_DETECTED );
  }
  else {
    setColor ( CUP_LED_START + cupId, COLOR_OFF );
  }
  rgbLeds.show ();
}


