#include <Adafruit_NeoPixel.h>
//#include <avr/power.h>

#define  LIMIT_SWITCH_PIN            8
#define  HOME_SWITCH_PIN             9

#define  DISPENSER_DIRECTION_PIN     10
#define  DISPENSER_STEP_PIN          11
#define  NUMBER_OF_STEPS_PER_REV     200
#define  MAXIMUM_NUMBER_OF_STEPS     7800 // 7864, 7961, 7953, 7838

#define  DIRECTIION_TO_HOME_POSITION  LOW
#define  DIRECTIION_TO_CUP_POSITION   HIGH

#define  DISTANCE_BETWEEN_CUPS       30

#define  LED_PIN    12
#define  LED_COUNT  ( CUP_POSITIONS * 2 )

#define  EMPTY_COLOR  Color ( 150, 0, 0 )

Adafruit_NeoPixel rgbLeds = Adafruit_NeoPixel ( LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800 );

bool isAtLimit = false;
bool isAtHome = false;
bool stopMotorMovement = false;
int maximumNumberOfSteps = 0;

int dispenserCurrentPosition = 0;

void setupCupController() {
  
  pinMode ( LIMIT_SWITCH_PIN, INPUT );
  pinMode ( HOME_SWITCH_PIN, INPUT );
  attachInterrupt ( LIMIT_SWITCH_PIN, limitSwitchStateChanged, CHANGE );
  attachInterrupt ( HOME_SWITCH_PIN, homeSwitchStateChanged, CHANGE );
  
  pinMode ( DISPENSER_DIRECTION_PIN, OUTPUT ); 
  pinMode ( DISPENSER_STEP_PIN, OUTPUT ); 

  rgbLeds.begin();
  
  goHome ();
  goMaxLimit ();
}

#define BUTTON_SENSITIVITY_MILLIS  5
elapsedMillis limitSwitchTimeSinceLastInterrupt = 0;
void limitSwitchStateChanged () {

  if ( limitSwitchTimeSinceLastInterrupt > BUTTON_SENSITIVITY_MILLIS ) {
    
    if ( digitalRead ( LIMIT_SWITCH_PIN ) == LOW ) { // Button Pressed
      stopMotorMovement = true;
      isAtLimit = true;
      maximumNumberOfSteps = dispenserCurrentPosition;
    }
    else {
      isAtLimit = false;
    }
    
    limitSwitchTimeSinceLastInterrupt = 0;
  }
}

elapsedMillis homeSwitchTimeSinceLastInterrupt = 0;
void homeSwitchStateChanged () {

  if ( homeSwitchTimeSinceLastInterrupt > BUTTON_SENSITIVITY_MILLIS ) {
    
    if ( digitalRead ( HOME_SWITCH_PIN ) == LOW ) { // Button Pressed
      stopMotorMovement = true;
      isAtHome = true;
      dispenserCurrentPosition = 0;
    }
    else {
      isAtHome = false;
    }
    
    homeSwitchTimeSinceLastInterrupt = 0;
  }
}

#define STEPPER_MOTOR_DELAY_MICRO_SECS  250
unsigned int moveDispenserHead ( unsigned int steps, int direction ) {
  
  int delta = 1;
  
  if ( direction == DIRECTIION_TO_HOME_POSITION ) {
    delta = -1; // reduce steps moving towards Home
  }
  
  digitalWrite ( DISPENSER_DIRECTION_PIN, direction );

  unsigned int stepsTaken = 0;
  
  while ( !stopMotorMovement && ( stepsTaken < steps ) ) {

    digitalWrite ( DISPENSER_STEP_PIN, HIGH );
    digitalWrite ( DISPENSER_STEP_PIN, LOW );
    
    stepsTaken ++;
    dispenserCurrentPosition += delta;

    delayMicroseconds ( STEPPER_MOTOR_DELAY_MICRO_SECS ); // To prevent motor from stalling
  }
  
  stopMotorMovement = false; // reset the movement flag.  Let the limit/home triggers handle the setting part

  return stepsTaken;
}

void markCupEmpty ( int cupID ) {
  rgbLeds.setPixelColor ( cupID, rgbLeds.EMPTY_COLOR );
  rgbLeds.show();
}

int currCupPosition = -1;

void goHome () {
  
  unsigned int stepsTaken = 0;
  while ( isAtHome == false ) {
    stepsTaken += moveDispenserHead ( MAXIMUM_NUMBER_OF_STEPS, DIRECTIION_TO_HOME_POSITION );
  }
  
  #if defined( SOFTWARE_DEBUG )
    Serial.print ( "Reached Home: [" );
    Serial.print ( isAtHome );
    Serial.print ( isAtLimit );
    Serial.print ( "] - at position: " );
    Serial.print ( dispenserCurrentPosition );
    Serial.print ( " - after taking steps: " );
    Serial.println ( stepsTaken );
  #endif
}

void goMaxLimit () {

  unsigned int stepsTaken = 0;
  while ( isAtLimit == false ) {
    stepsTaken += moveDispenserHead ( MAXIMUM_NUMBER_OF_STEPS, DIRECTIION_TO_CUP_POSITION );
  }
  
  #if defined( SOFTWARE_DEBUG )
    Serial.print ( "Reached Limit: [" );
    Serial.print ( isAtHome );
    Serial.print ( isAtLimit );
    Serial.print ( "] - at position: " );
    Serial.print ( dispenserCurrentPosition );
    Serial.print ( " - after taking steps: " );
    Serial.println ( stepsTaken );
  #endif
}

void goCupPosition ( int cupPos ) {
  
  #if defined( SOFTWARE_DEBUG )
    Serial.print ( "Trying to reach cup Position: " );
    Serial.println ( cupPos );
  #endif
  
  goHome ();
  
  int cupPositionLength = maximumNumberOfSteps / ( CUP_POSITIONS + 1 );
  
  int steps = cupPositionLength * ( cupPos + 1 );
  
  unsigned int stepsTaken = moveDispenserHead ( steps, DIRECTIION_TO_CUP_POSITION );

  #if defined( SOFTWARE_DEBUG )
    Serial.print ( "Now at Cup Poistion: [" );
    Serial.print ( isAtHome );
    Serial.print ( isAtLimit );
    Serial.print ( "] - " );
    Serial.print ( cupPos );
    Serial.print ( " - at position: " );
    Serial.print ( dispenserCurrentPosition );
    Serial.print ( " - after taking steps: " );
    Serial.println ( stepsTaken );
  #endif
  
}

bool goToNextCupPosition () {
  currCupPosition = ( currCupPosition + 1 ) % CUP_POSITIONS;
  goCupPosition ( currCupPosition );
  
  return true;
}

