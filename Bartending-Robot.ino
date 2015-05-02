

// comment the following define statement to shut the built-in LED off
#define USE_DEBUG_LED 1

// Comment the following to skip Serial Port Setup
//#define SOFTWARE_DEBUG 1

#define NUMBER_OF_BUTTONS  12

#define ORDER_QUEUE_SIZE  4

const int buttonPins [ NUMBER_OF_BUTTONS ] = { 10, 11, 12, 14, 15, 16, 17, 18, 19, 20, 21, 22 };
const int orderQueue [ ORDER_QUEUE_SIZE ] = { -1 };

void setup() {
  // put your setup code here, to run once:
  
  int i = 0;
  for ( i = 0; i < NUMBER_OF_BUTTONS; i ++ ) {
    pinMode ( buttonPins [ i ], INPUT_PULLUP );
    attachInterrupt ( buttonPins [ i ], pinChanged, CHANGE );
  }

  // Debug LED setup
#if defined( USE_DEBUG_LED )
  pinMode ( LED_BUILTIN, OUTPUT );
#endif
  
#if defined( SOFTWARE_DEBUG )
  Serial.begin(9600);
  delay(2000);// Give reader a chance to see the output.
#endif

}

#define BUTTON_SENSITIVITY_MILLIS  3

elapsedMillis millisSinceInterrupt = 0;
volatile boolean inInterrupt = false;
volatile int menuID = -1;

void pinChanged () {
  
  if ( !inInterrupt ) {
    
    inInterrupt = true;
    
    if ( millisSinceInterrupt > BUTTON_SENSITIVITY_MILLIS ) {
  
      for ( int i = 0; i < NUMBER_OF_BUTTONS; i ++ ) {
        
        boolean value = ( digitalRead ( buttonPins [ i ] ) == LOW );
        
        if ( value == true ) {
          menuID = i;
        }
      }
    }

    inInterrupt = false;
    millisSinceInterrupt = 0;
  }
}

int previousMenuID = -1;

#if defined( USE_DEBUG_LED )
bool builtInLedState = HIGH;
#endif

void loop() {

  if ( previousMenuID != menuID ) {
    Serial.print ( "Selected Menu: " );
    Serial.println ( menuID );
  }
  
  previousMenuID = menuID;

#if defined( USE_DEBUG_LED )
  builtInLedState = !builtInLedState;
  digitalWrite ( LED_BUILTIN, builtInLedState );
  delay ( 250 );
#endif

}
