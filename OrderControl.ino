#include <BarvizQueue.h>

#define NUMBER_OF_BUTTONS  8
#define NUMBER_OF_RECIPIES NUMBER_OF_BUTTONS - 1

const int buttonPins [ NUMBER_OF_BUTTONS ] = { 0, 1, 2, 3, 4, 5, 6, 7 };

const int DRINK_RECIPIES [ NUMBER_OF_RECIPIES ] [ PUMP_COUNT ] = {
  { 1, 2, 3, 4, 5, 6 },
  { 2, 3, 4, 5, 6, 7 },
  { 3, 4, 5, 6, 1, 2 },
  { 4, 5, 6, 7, 1, 2 },
  { 1, 0, 0, 2, 1, 0 },
  { 3, 2, 1, 0, 1, 0 },
  { 0, 2, 5, 0, 1, 4 },
};

typedef struct {
  int recipieIndex;
  String toString ();
} Order;

String Order :: toString () {
  String str = "Recipie #: ";
  str += recipieIndex;
  str += " { ";
  for ( int i = 0; i < PUMP_COUNT; i ++ ) {
    if ( i > 0 ) {
      str += ", ";
    }
    str += DRINK_RECIPIES [ recipieIndex ] [ i ];
  }
  str += " }";
  return str;
}

BarvizQueue <Order> orderQue ( ORDER_QUEUE_SIZE );

void setupOrderController () {
  
  int i = 0;
  for ( i = 0; i < NUMBER_OF_RECIPIES; i ++ ) {
    pinMode ( buttonPins [ i ], INPUT_PULLUP );
    attachInterrupt ( buttonPins [ i ], menuButtonPinChanged, CHANGE );
  }
  
  pinMode ( buttonPins [ NUMBER_OF_BUTTONS - 1 ], INPUT_PULLUP );
  attachInterrupt ( buttonPins [ i ], clearRecentRecipie, CHANGE );
  
  #if defined( SOFTWARE_DEBUG )
    orderQue.setPrinter ( Serial );
  #endif
}

#define BUTTON_SENSITIVITY_MILLIS  300

elapsedMillis millisSinceInterrupt = 0;

void menuButtonPinChanged () {
  
  if ( millisSinceInterrupt > BUTTON_SENSITIVITY_MILLIS ) {

    for ( int i = 0; i < NUMBER_OF_RECIPIES; i ++ ) {
      
      boolean value = ( digitalRead ( buttonPins [ i ] ) == LOW );
      
      if ( value == true ) {
        if ( !orderQue.isFull () ) {
          Order * order = new Order ();
          order -> recipieIndex = i;
          orderQue.add ( order );
        }
      }
    }
  
    millisSinceInterrupt = 0;
  }
}

void clearRecentRecipie () {

  if ( millisSinceInterrupt > BUTTON_SENSITIVITY_MILLIS ) {
    
    if ( ! orderQue.isEmpty () ) {
      Order * order = orderQue.removeMostRecent ();
      delete order;
    }
    
    millisSinceInterrupt = 0;
  }
}


void processOrder () {

  if ( !orderQue.isEmpty () ) {
  
    Order * order = orderQue.remove ();
    
    if ( order != NULL ) {
      executeRecipie ( DRINK_RECIPIES [ order -> recipieIndex ] );
    }
  }
}

int previousQueueSize = -1;

void orderControlDemo () {

  if ( previousQueueSize != orderQue.size () ) {
    #if defined( SOFTWARE_DEBUG )
      Serial.println ( "Que Size changed: " );
      Serial.println ( orderQue.size () );
      orderQue.print ();
    #endif
  }

  previousQueueSize = orderQue.size();
}

