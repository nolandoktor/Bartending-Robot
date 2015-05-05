#include <BarvizQueue.h>

// comment the following define statement to shut the built-in LED off
#define USE_DEBUG_LED 1

// Comment the following to skip Serial Port Setup
//#define SOFTWARE_DEBUG 1

#define NUMBER_OF_BUTTONS  12

#define ORDER_QUEUE_SIZE  4

const int buttonPins [ NUMBER_OF_BUTTONS ] = { 10, 11, 12, 14, 15, 16, 17, 18, 19, 20, 21, 22 };

typedef struct {
  int orderIndex;
  
  String toString ();
} Order;

String Order :: toString () {
  return " Testing";
}

BarvizQueue <Order> orderQue ( ORDER_QUEUE_SIZE );

void setup() {
  
  int i = 0;
  for ( i = 0; i < NUMBER_OF_BUTTONS; i ++ ) {
    pinMode ( buttonPins [ i ], INPUT_PULLUP );
    attachInterrupt ( buttonPins [ i ], pinChanged, CHANGE );
  }
  
  setupPumpConroller ();

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

#define BUTTON_SENSITIVITY_MILLIS  250

elapsedMillis millisSinceInterrupt = 0;

void pinChanged () {
  
  if ( millisSinceInterrupt > BUTTON_SENSITIVITY_MILLIS ) {

    for ( int i = 0; i < NUMBER_OF_BUTTONS; i ++ ) {
      
      boolean value = ( digitalRead ( buttonPins [ i ] ) == LOW );
      
      if ( value == true ) {
        if ( !orderQue.isFull () ) {
          Order * order = new Order ();
          order -> orderIndex = i;
          orderQue.add ( order );
        }
      }
    }
  }

  millisSinceInterrupt = 0;
}

#if defined( USE_DEBUG_LED )
bool builtInLedState = HIGH;
#endif

int previousQueueSize = 0;

void loop() {

  if ( previousQueueSize != orderQue.size () ) {
    orderQue.print ();
  }
  
  previousQueueSize = orderQue.size();

#if defined( USE_DEBUG_LED )
  builtInLedState = !builtInLedState;
  digitalWrite ( LED_BUILTIN, builtInLedState );
  delay ( 250 );
#endif

}
