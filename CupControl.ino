#include <Adafruit_NeoPixel.h>
//#include <avr/power.h>

#define  END_SWITCH_PIN              8
#define  HOME_SWITCH_PIN             9

#define  DISPENSER_DIRECTION_PIN     10
#define  DISPENSER_STEP_PIN          11
#define  NUMBER_OF_STEPS_PER_REV     200
#define  MAXIMUM_NUMBER_OF_STEPS     7800 // 7864, 7961, 7953, 7838

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
  attachInterrupt ( HOME_SWITCH_PIN, homeSwitchStateChanged, CHANGE );
  
  pinMode ( DISPENSER_DIRECTION_PIN, OUTPUT ); 
  pinMode ( DISPENSER_STEP_PIN, OUTPUT ); 

  rgbLeds.begin();
  
  goToHome ();
  goToEnd ();
  moveDispenserHead ( ( maximumNumberOfSteps / 2 ), DIR_TO_HOME );
}

#define STEPPER_MOTOR_DELAY_MICRO_SECS  500
#define BUTTON_SENSITIVITY_MICROS  STEPPER_MOTOR_DELAY_MICRO_SECS

elapsedMicros limitSwitchTimeSinceLastInterrupt = 0;
void limitSwitchStateChanged () {
  cli ();
  if ( limitSwitchTimeSinceLastInterrupt >= BUTTON_SENSITIVITY_MICROS ) {
    if ( digitalRead ( END_SWITCH_PIN ) == LOW ) { // Button Pressed
      stopMotorMovement = true;
      isAtEnd = true;
      maximumNumberOfSteps = dispenserCurrentPosition;
    }
    else {
      isAtEnd = false;
    }
    limitSwitchTimeSinceLastInterrupt = 0;
  }
  sei ();
}

elapsedMicros homeSwitchTimeSinceLastInterrupt = 0;
void homeSwitchStateChanged () {
  cli ();
  if ( homeSwitchTimeSinceLastInterrupt >= BUTTON_SENSITIVITY_MICROS ) {
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
  sei ();
}

unsigned int moveDispenserHead ( unsigned int steps, int direction ) {
  
  if ( isAtHome && direction == DIR_TO_HOME ) {
    return 0;
  }
  
  if ( isAtEnd && direction == DIR_TO_END ) {
    return 0;
  }
  
  int delta = 1;
  if ( direction == DIR_TO_HOME ) {
    delta = -1; // reduce steps moving towards Home
  }
  
  digitalWrite ( DISPENSER_DIRECTION_PIN, direction );

  unsigned int stepsTaken = 0;
  
  while ( !stopMotorMovement && ( stepsTaken < steps ) ) {
    
    cli ();
    digitalWrite ( DISPENSER_STEP_PIN, HIGH );
    digitalWrite ( DISPENSER_STEP_PIN, LOW );
    stepsTaken ++;
    dispenserCurrentPosition += delta;
    sei ();

    delayMicroseconds ( STEPPER_MOTOR_DELAY_MICRO_SECS ); // To prevent motor from stalling
  }
  
  if ( isAtHome && ( direction == DIR_TO_END ) && ( stepsTaken > 0 ) ) {
    isAtHome = false;
  }
  
  if ( isAtEnd && ( direction == DIR_TO_HOME ) && ( stepsTaken > 0 ) ) {
    isAtEnd = false;
  }

  stopMotorMovement = false; // reset the movement flag.  Let the limit/home triggers handle the setting part

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
    stepsTaken += moveDispenserHead ( MAXIMUM_NUMBER_OF_STEPS, DIR_TO_HOME );
  }
  
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
    stepsTaken += moveDispenserHead ( MAXIMUM_NUMBER_OF_STEPS, DIR_TO_END );
  }
  
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
  
  #if defined( SOFTWARE_DEBUG )
    Serial.print ( "Trying to reach cup Position: " );
    Serial.println ( cupPos );
  #endif
  
  goToHome ();
  
  int cupPositionLength = maximumNumberOfSteps / ( CUP_POSITIONS + 1 );
  int steps = cupPositionLength * ( cupPos + 1 );
  unsigned int stepsTaken = moveDispenserHead ( steps, DIR_TO_END );

  #if defined( SOFTWARE_DEBUG )
    Serial.print ( "Now at Cup Poistion: [" );
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

