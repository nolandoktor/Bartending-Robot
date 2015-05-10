#include <Adafruit_NeoPixel.h>

#define  END_SWITCH_PIN              8
#define  HOME_SWITCH_PIN             9

#define  DISPENSER_DIRECTION_PIN     10
#define  DISPENSER_STEP_PIN          11
#define  NUMBER_OF_STEPS_PER_REV     200
//#define  MAXIMUM_NUMBER_OF_STEPS     7800 // 7864, 7961, 7953, 7838

#define  DIR_TO_HOME  LOW
#define  DIR_TO_END   HIGH

#define  LED_PIN    12
#define  LED_COUNT  ( CUP_POSITIONS * 2 )

#define  EMPTY_COLOR  Color ( 150, 0, 0 )

Adafruit_NeoPixel rgbLeds = Adafruit_NeoPixel ( LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800 );

bool isAtEnd = false;
bool isAtHome = false;
bool stopMotorMovement = false;
int maximumNumberOfSteps = 0;

int dispenserCurrentPosition = 0;

void setupCupController() {
  
  pinMode ( END_SWITCH_PIN, INPUT );
  pinMode ( HOME_SWITCH_PIN, INPUT );
  attachInterrupt ( END_SWITCH_PIN, limitSwitchStateChanged, CHANGE );
  attachInterrupt ( HOME_SWITCH_PIN, limitSwitchStateChanged, CHANGE );
//  attachInterrupt ( HOME_SWITCH_PIN, homeSwitchStateChanged, CHANGE );
  
  pinMode ( DISPENSER_DIRECTION_PIN, OUTPUT ); 
  pinMode ( DISPENSER_STEP_PIN, OUTPUT ); 

  rgbLeds.begin();
  
  goToEnd ();
  goToHome ();
  goToEnd ();
  moveDispenserHead ( ( maximumNumberOfSteps / 2 ), DIR_TO_HOME );
}

#define STEPPER_MOTOR_DELAY_MICRO_SECS  250
#define BUTTON_SENSITIVITY              5 

bool inStep = false;
bool interruptWaiting = false;

elapsedMillis limitSwitchTimeSinceLastInterrupt = 0;
void limitSwitchStateChanged () {
  if ( inStep ) {
    interruptWaiting = true; // Let the stepper know that there is interrupt in wait
  }
  else {
    interruptWaiting = false;
    if ( limitSwitchTimeSinceLastInterrupt >= BUTTON_SENSITIVITY ) {
      
      if ( digitalRead ( END_SWITCH_PIN ) == LOW ) { // Button Pressed
        stopMotorMovement = true;
        isAtEnd = true;
        maximumNumberOfSteps = dispenserCurrentPosition;
      }
      else {
        isAtEnd = false;
      }

      if ( digitalRead ( HOME_SWITCH_PIN ) == LOW ) { // Button Pressed
        stopMotorMovement = true;
        isAtHome = true;
        dispenserCurrentPosition = 0;
      }
      else {
        isAtHome = false;
      }
      
      limitSwitchTimeSinceLastInterrupt = 0;
    }
  }
}

unsigned int moveDispenserHead ( unsigned int steps, int direction ) {
  
  int delta = 1;
  if ( direction == DIR_TO_HOME ) {
    delta = -1; // reduce steps moving towards Home
  }
  
  digitalWrite ( DISPENSER_DIRECTION_PIN, direction );

  unsigned int stepsTaken = 0;
  
  while ( !stopMotorMovement && ( stepsTaken < steps ) ) {
    
    inStep = true;
    digitalWrite ( DISPENSER_STEP_PIN, HIGH );
    digitalWrite ( DISPENSER_STEP_PIN, LOW );
    stepsTaken ++;
    dispenserCurrentPosition += delta;
    inStep = false;

    if ( interruptWaiting ) {
      delay ( STEPPER_MOTOR_DELAY_MICRO_SECS ); // Wait in milliSeconds, whenever any of the limit-switch triggers
    }
    else {
      delayMicroseconds ( STEPPER_MOTOR_DELAY_MICRO_SECS ); // To prevent motor from stalling
    }
  }
  
  if ( stepsTaken > 0 ) {
    if ( direction == DIR_TO_END && isAtHome ) {
      isAtHome = false;
    }
    if ( direction == DIR_TO_HOME && isAtEnd ) {
      isAtEnd = false;
    }
  }
  
  return stepsTaken;
}

void markCupEmpty ( int cupID ) {
  rgbLeds.setPixelColor ( cupID, rgbLeds.EMPTY_COLOR );
  rgbLeds.show();
}

int currCupPosition = -1;

void goToHome () {
  
  unsigned int stepsTaken = 0;
  while ( isAtHome == false ) {
    stepsTaken += moveDispenserHead ( NUMBER_OF_STEPS_PER_REV, DIR_TO_HOME );
  }
  
  stopMotorMovement = false; // reset the movement flag.  Let the limit/home triggers handle the setting part

  #if defined( SOFTWARE_DEBUG )
    Serial.print ( "Reached Home: [" );
    Serial.print ( isAtHome );
    Serial.print ( isAtEnd );
    Serial.print ( "] - at position: " );
    Serial.print ( dispenserCurrentPosition );
    Serial.print ( " - after taking steps: " );
    Serial.println ( stepsTaken );
  #endif
}

void goToEnd () {

  unsigned int stepsTaken = 0;
  while ( isAtEnd == false ) {
    stepsTaken += moveDispenserHead ( NUMBER_OF_STEPS_PER_REV, DIR_TO_END );
  }
  
  stopMotorMovement = false; // reset the movement flag.  Let the limit/home triggers handle the setting part

  #if defined( SOFTWARE_DEBUG )
    Serial.print ( "Reached Limit: [" );
    Serial.print ( isAtHome );
    Serial.print ( isAtEnd );
    Serial.print ( "] - at position: " );
    Serial.print ( dispenserCurrentPosition );
    Serial.print ( " - after taking steps: " );
    Serial.println ( stepsTaken );
  #endif
}

void goToCup ( int cupPos ) {
  
  goToHome ();
  
  int cupPositionLength = maximumNumberOfSteps / ( CUP_POSITIONS + 1 );
  int steps = cupPositionLength * ( cupPos + 1 );
  unsigned int stepsTaken = moveDispenserHead ( steps, DIR_TO_END );

  #if defined( SOFTWARE_DEBUG )
    Serial.print ( "Cup Poistion: [" );
    Serial.print ( isAtHome );
    Serial.print ( isAtEnd );
    Serial.print ( "] - " );
    Serial.print ( cupPos );
    Serial.print ( " - at position: " );
    Serial.print ( dispenserCurrentPosition );
    Serial.print ( " - after taking steps: " );
    Serial.println ( stepsTaken );
  #endif
}

bool inMotion = false;
bool goToNextCupPosition () {

  if ( !inMotion ) {
    inMotion = true;

    currCupPosition = ( currCupPosition + 1 ) % CUP_POSITIONS;
    goToCup ( currCupPosition );
    
    inMotion = false;
    return true;
  }
  else {
    return false;
  }
}

