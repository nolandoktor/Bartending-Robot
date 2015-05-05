#include <BarvizQueue.h>

#define NUMBER_OF_BUTTONS  12

#define ORDER_QUEUE_SIZE  4

const int buttonPins [ NUMBER_OF_BUTTONS ] = { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 44, 45 };

typedef struct {
  int orderIndex;
  
  String toString ();
} Order;

String Order :: toString () {
  return " Testing";
}

BarvizQueue <Order> orderQue ( ORDER_QUEUE_SIZE );

void setupOrderController () {
  
  int i = 0;
  for ( i = 0; i < NUMBER_OF_BUTTONS; i ++ ) {
    pinMode ( buttonPins [ i ], INPUT_PULLUP );
    attachInterrupt ( buttonPins [ i ], menuButtonPinChanged, CHANGE );
  }
  
  #if defined( SOFTWARE_DEBUG )
    orderQue.setPrinter ( Serial );
  #endif
}

#define BUTTON_SENSITIVITY_MILLIS  250

elapsedMillis millisSinceInterrupt = 0;

void menuButtonPinChanged () {
  
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

int previousQueueSize = -1;

void orderControlMainRoutine () {

  if ( previousQueueSize != orderQue.size () ) {
    #if defined( SOFTWARE_DEBUG )
      Serial.println ( "Que Size changed: " );
      Serial.println ( orderQue.size () );
      orderQue.print ();
    #endif
  }

  previousQueueSize = orderQue.size();
}

