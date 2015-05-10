// comment the following define statement to shut the built-in LED off
#define USE_DEBUG_LED      1

// Comment the following to skip Serial Port Setup
#define SOFTWARE_DEBUG     1

#define PUMP_COUNT         6
#define ORDER_QUEUE_SIZE   4
#define CUP_POSITIONS      ORDER_QUEUE_SIZE

#if defined( USE_DEBUG_LED )
  IntervalTimer debugLedTimer;
#endif

volatile unsigned int globalPumpStatus = 0;

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
  setupCupController ();
}

#if defined( USE_DEBUG_LED )
  bool builtInLedState = HIGH;
  unsigned int previousPumpState = 1;
  void debugLedTimerRoutine () {
    builtInLedState = !builtInLedState;
    digitalWrite ( LED_BUILTIN, builtInLedState );

    if ( previousPumpState != globalPumpStatus ) {
      Serial.print ( "Current pump satus: " );
      Serial.println ( globalPumpStatus );
    }
    previousPumpState = globalPumpStatus;
  }
#endif

void loop() {

  if ( globalPumpStatus == 0 ) {

    const int * recipie = getNextOrder ();
    
    if ( recipie != NULL ) {
      
      if ( goToNextCupPosition () ) {
        
        // if ( isCupPresent () )

        runPumpsFor ( recipie );
        orderProcessed ();
      }
    }
    else {
      goToNextCupPosition ();
    }
  }
  
//  markCupEmpty ( 0 );
//  orderControlDemo ();
//  pumpControlDemo ();

}
