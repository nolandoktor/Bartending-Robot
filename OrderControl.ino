#include <BarvizQueue.h>

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

void setupOrderController () {
  int i = 0;
  for ( i = 0; i < NUMBER_OF_BUTTONS; i ++ ) {
    pinMode ( buttonPins [ i ], INPUT_PULLUP );
    attachInterrupt ( buttonPins [ i ], menuButtonPinChanged, CHANGE );
  }
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

int previousQueueSize = 0;

void orderControlMainRoutine () {

  if ( previousQueueSize != orderQue.size () ) {
    orderQue.print ();
  }

  previousQueueSize = orderQue.size();
}

