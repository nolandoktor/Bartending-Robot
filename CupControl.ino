#include <Adafruit_NeoPixel.h>
//#include <avr/power.h>

#define  HOME_SWITCH_PIN             8

#define  DISPENSER_DIRECTION_PIN     10
#define  DISPENSER_STEP_PIN          11
#define  NUMBER_OF_STEPS_PER_REV     200
#define  MAXIMUM_NUMBER_OF_STEPS     200


#define  DIRECTIION_TO_HOME_POSITION  HIGH
#define  DIRECTIION_TO_CUP_POSITION   LOW

#define  DISTANCE_BETWEEN_CUPS       30


#define  LED_PIN    12
#define  LED_COUNT  ( CUP_POSITIONS * 2 )

#define  EMPTY_COLOR  Color ( 150, 0, 0 )

Adafruit_NeoPixel rgbLeds = Adafruit_NeoPixel ( LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800 );

void setupCupController() {
  
  pinMode ( HOME_SWITCH_PIN, INPUT );
  attachInterrupt ( HOME_SWITCH_PIN, homeSwitchStateChanged, LOW );
  
  pinMode ( DISPENSER_DIRECTION_PIN, OUTPUT ); 
  pinMode ( DISPENSER_STEP_PIN, OUTPUT ); 

  rgbLeds.begin();
  
  goToNextCupPosition ();
}

bool isAtHome = false;
bool stopMotorMovement = false;

#define BUTTON_SENSITIVITY_MILLIS  300
elapsedMillis millisSincehomeSwitchStateChanged = 0;

void homeSwitchStateChanged () {
  
  if ( millisSincehomeSwitchStateChanged > BUTTON_SENSITIVITY_MILLIS ) {
    
    int pinValue = digitalRead ( HOME_SWITCH_PIN );
  
    stopMotorMovement |= ( pinValue == LOW );
  
    if ( stopMotorMovement && !( isAtHome ) ) {
      // Only do it once here, let moveDispenserHead() handle the ( isAtHome = false ) condition
      isAtHome = true;
    }
  
    Serial.print ( "Home Switch Value: " );
    Serial.print ( pinValue );
    Serial.print ( " - " );
    Serial.print ( stopMotorMovement );
    Serial.print ( " - " );
    Serial.println ( isAtHome );
    
    millisSincehomeSwitchStateChanged = 0;
  }
}

bool moveDispenserHead ( unsigned int steps, int direction ) {
  
  bool retVal = false;

  bool resetHome = false;
  
  if ( isAtHome ) {
    if ( direction != DIRECTIION_TO_CUP_POSITION ) {
      // Dispenser is already at home, do not move further down direction at all
      stopMotorMovement = true;
    }
    else {
      // if there is a single step towards the cup direction, make sure to mark isAtHome as false
      resetHome = true;
    }
  }
  
  digitalWrite ( DISPENSER_DIRECTION_PIN, direction );

  unsigned int stepsTaken = 0;
  
  while ( stepsTaken < steps ) {

    if ( stopMotorMovement ) {
      retVal = false;
      break;
    }
    else {
      digitalWrite ( DISPENSER_STEP_PIN, HIGH );
      digitalWrite ( DISPENSER_STEP_PIN, LOW );
      
      if ( resetHome ) {
        // Motor moved and is not at home anymore (moved towards the cups)
        isAtHome = false;
      }
    }
    
    stepsTaken ++;
  }
  
  retVal = ( stepsTaken == steps ); // if the loop made it through all the steps, it's been a successfull move
  
  stopMotorMovement = false;
  
  return retVal;
}

void markCupEmpty ( int cupID ) {
  rgbLeds.setPixelColor ( cupID, rgbLeds.EMPTY_COLOR );
  rgbLeds.show();
}

int currCupPosition = -1;

void goHome () {
  
  while ( !isAtHome ) {
    moveDispenserHead ( MAXIMUM_NUMBER_OF_STEPS, DIRECTIION_TO_HOME_POSITION );
  }
}

void goCupPosition ( int cupPos ) {
  
}

void goToNextCupPosition () {
  currCupPosition = ( currCupPosition + 1 ) % CUP_POSITIONS;
  goHome ();
  goCupPosition ( currCupPosition );
}

