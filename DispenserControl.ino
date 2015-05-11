#define  END_SWITCH_PIN              8
#define  HOME_SWITCH_PIN             9

#define  DISPENSER_DIRECTION_PIN     10
#define  DISPENSER_STEP_PIN          11
#define  NUMBER_OF_STEPS_PER_REV     200
//#define  MAXIMUM_NUMBER_OF_STEPS     7800 // 7864, 7961, 7953, 7838

#define  DIR_TO_HOME  LOW
#define  DIR_TO_END   HIGH

bool isAtEnd = false;
bool isAtHome = false;
int maximumNumberOfSteps = 0;
int dispenserCurrentPosition = 0;

void setupDispenserController() {
  
  pinMode ( END_SWITCH_PIN, INPUT );
  pinMode ( HOME_SWITCH_PIN, INPUT );
  
  pinMode ( DISPENSER_DIRECTION_PIN, OUTPUT ); 
  pinMode ( DISPENSER_STEP_PIN, OUTPUT ); 

  goToEnd ();
  goToHome ();
  goToEnd ();
  maximumNumberOfSteps = dispenserCurrentPosition;
  moveDispenserHead ( ( maximumNumberOfSteps / 2 ), DIR_TO_HOME );
}

#define STEPPER_MOTOR_DELAY_MICRO_SECS  250
void testHomeEndSwitches () {
  if ( digitalRead ( HOME_SWITCH_PIN ) == LOW  ) { // Home Button Pressed
    isAtHome = true;
    dispenserCurrentPosition = 0;
  }
  else {
    isAtHome = false;
  }

  if ( digitalRead ( END_SWITCH_PIN ) == LOW ) { // End Button Pressed
    isAtEnd = true;
    maximumNumberOfSteps = dispenserCurrentPosition;
  }
  else {
    isAtEnd = false;
  }
}

unsigned int moveDispenserHead ( unsigned int steps, int direction ) {
  
  int delta = 1;
  if ( direction == DIR_TO_HOME ) {
    delta = -1; // reduce steps moving towards Home
  }
  
  digitalWrite ( DISPENSER_DIRECTION_PIN, direction );

  bool stopMotorMovement = false;
  unsigned int stepsTaken = 0;
  
  while ( !stopMotorMovement && ( stepsTaken < steps ) ) {
    
    testHomeEndSwitches ();

    if ( isAtHome && ( direction == DIR_TO_HOME ) ) {
      stopMotorMovement = true;
    }
    else if ( isAtEnd && ( direction == DIR_TO_END ) ) {
      stopMotorMovement = true;
    }
      
    if ( !stopMotorMovement ) {
      digitalWrite ( DISPENSER_STEP_PIN, HIGH );
      digitalWrite ( DISPENSER_STEP_PIN, LOW );
      stepsTaken ++;
      dispenserCurrentPosition += delta;
      delayMicroseconds ( STEPPER_MOTOR_DELAY_MICRO_SECS ); // To prevent motor from stalling
    }
  }

  return stepsTaken;
}

int currCupPosition = -1;

void goToHome () {
  
  unsigned int stepsTaken = 0;
  while ( isAtHome == false ) {
    stepsTaken += moveDispenserHead ( NUMBER_OF_STEPS_PER_REV, DIR_TO_HOME );
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
    stepsTaken += moveDispenserHead ( NUMBER_OF_STEPS_PER_REV, DIR_TO_END );
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
  
  goToHome ();
  
  moveDispenserHead ( dispenserCurrentPosition - 50, DIR_TO_HOME );
  
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

