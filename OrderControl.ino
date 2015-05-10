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
        if ( !orderQue.isFull () ) {
          orderQue.add ( new int ( i ) );
        }
      }
    }
  
    millisSinceInterrupt = 0;
  }
}

void clearRecentRecipie () {

  if ( millisSinceInterrupt > BUTTON_SENSITIVITY_MILLIS ) {
    
    if ( ! orderQue.isEmpty () ) {
      int * order = orderQue.removeMostRecent ();
      delete order;
    }
    
    millisSinceInterrupt = 0;
  }
}


const int * getNextOrder () {
  
  if ( !orderQue.isEmpty () ) {
  
    int * order = orderQue.peek ();
    
    if ( order != NULL ) {
      return ( DRINK_RECIPIES [ *( order ) ] );
    }
  }
  
  return NULL;
}

void orderProcessed () {
  
  if ( !orderQue.isEmpty () ) {
    
    int * order = orderQue.remove ();
    
    if ( order != NULL ) {
      delete order;
    }
  }
}

int previousQueueSize = -1;

void orderControlDemo () {

  if ( previousQueueSize != orderQue.size () ) {
    #if defined( SOFTWARE_DEBUG )
      Serial.print ( "Que Size: " );
      Serial.println ( orderQue.size () );
    #endif
  }

  previousQueueSize = orderQue.size();
}

