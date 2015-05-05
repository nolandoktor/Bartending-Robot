#include <BarvizQueue.h>

// comment the following define statement to shut the built-in LED off
#define USE_DEBUG_LED 1

// Comment the following to skip Serial Port Setup
#define SOFTWARE_DEBUG 1

#define PUMP_COUNT 6
#define ORDER_QUEUE_SIZE   4

#if defined( USE_DEBUG_LED )
  IntervalTimer debugLedTimer;
#endif

void setup() {
  
  // Debug LED setup
#if defined( USE_DEBUG_LED )
  pinMode ( LED_BUILTIN, OUTPUT );
  debugLedTimer.begin ( debugLedTimerRoutine, 250000 );
#endif
  
#if defined( SOFTWARE_DEBUG )
  Serial.begin(9600);
  delay(2000);// Give reader a chance to see the output.
#endif

  setupPumpConroller ();
  setupOrderController ();
}

#if defined( USE_DEBUG_LED )
  bool builtInLedState = HIGH;
  
  void debugLedTimerRoutine () {
    builtInLedState = !builtInLedState;
    digitalWrite ( LED_BUILTIN, builtInLedState );
  }
#endif

void executeRecipie ( const int recipie [] ) {
  runPumpsFor ( PUMP_COUNT, recipie );
}

void loop() {

  if ( runningPumpsCount () == 0 ) {
    processOrder ();
  }

  orderControlDemo ();
//  pumpControlDemo ();

}
