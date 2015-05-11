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

BarvizQueue <int> orderQue ( ORDER_QUEUE_SIZE );

void setupOrderController () {
  
  int i = 0;
  for ( i = 0; i < NUMBER_OF_RECIPIES; i ++ ) {
    pinMode ( buttonPins [ i ], INPUT_PULLUP );
    attachInterrupt ( buttonPins [ i ], menuButtonPinChanged, CHANGE );
  }
  
  pinMode ( buttonPins [ NUMBER_OF_BUTTONS - 1 ], INPUT_PULLUP );
  attachInterrupt ( buttonPins [ i ], clearRecentRecipie, CHANGE );
}

#define BUTTON_SENSITIVITY_MILLIS  300

elapsedMillis millisSinceInterrupt = 0;
void menuButtonPinChanged () {
  
  if ( millisSinceInterrupt > BUTTON_SENSITIVITY_MILLIS ) {

    for ( int i = 0; i < NUMBER_OF_RECIPIES; i ++ ) {
      
      boolean value = ( digitalRead ( buttonPins [ i ] ) == LOW );
      
      if ( value == true ) {
        orderQue.add ( i );
        showOrderStatusBlinky ();
      }
    }
  
    millisSinceInterrupt = 0;
  }
}

void clearRecentRecipie () {

  if ( millisSinceInterrupt > BUTTON_SENSITIVITY_MILLIS ) {
    
    int order;
    orderQue.removeMostRecent ( order );
    showOrderStatusBlinky ();

    millisSinceInterrupt = 0;
  }
}

const int * getNextOrder () {
  
  if ( !orderQue.isEmpty () ) {
  
    int order;
    if ( orderQue.peek ( order ) ) {
      return ( DRINK_RECIPIES [ order ] );
    }
  }
  
  return NULL;
}

void orderProcessed () {
  
  if ( !orderQue.isEmpty () ) {
    
    int order;
    orderQue.remove ( order );
  }
  
  showOrderStatusBlinky ();
}

int getOrderQueSize () {
  return orderQue.size ();
}

int * getOrderQueStatus ( int * statusArray, int maxQueSize ) {

  int queSize = orderQue.asArray ( statusArray, maxQueSize );
  
  for ( int i = queSize; i < maxQueSize; i ++ ) {
      statusArray [ i ] = -1;
  }
  
  return statusArray;
}



