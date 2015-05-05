#include <BarvizQueue.h>

// comment the following define statement to shut the built-in LED off
#define USE_DEBUG_LED 1

// Comment the following to skip Serial Port Setup
//#define SOFTWARE_DEBUG 1

void setup() {
  
  setupPumpConroller ();
  setupOrderController ();

  // Debug LED setup
#if defined( USE_DEBUG_LED )
  pinMode ( LED_BUILTIN, OUTPUT );
#endif
  
#if defined( SOFTWARE_DEBUG )
  Serial.begin(9600);
  delay(2000);// Give reader a chance to see the output.
  orderQue.setPrinter ( Serial );
#endif

}

#if defined( USE_DEBUG_LED )
bool builtInLedState = HIGH;
#endif


void loop() {
  
  orderControlMainRoutine ();

#if defined( USE_DEBUG_LED )
  builtInLedState = !builtInLedState;
  digitalWrite ( LED_BUILTIN, builtInLedState );
  delay ( 250 );
#endif

}
