#define  END_SWITCH_PIN              8
#define  HOME_SWITCH_PIN             9

#define  DISPENSER_DIRECTION_PIN     11
#define  DISPENSER_STEP_PIN          12
#define  NUMBER_OF_STEPS_PER_REV     200

const int cupDetectionPins [ CUP_POSITIONS ] = { 14, 15, 16, 17 };
const unsigned int cupSteps [ CUP_POSITIONS ] = { 1408, 2928, 4422, 5918 };

//#define  MAXIMUM_NUMBER_OF_STEPS     7800 // 7864, 7961, 7953, 7838

#define  DIR_TO_HOME  LOW
#define  DIR_TO_END   HIGH

bool isAtEnd = false;
bool isAtHome = false;
int maximumNumberOfSteps = 0;
int dispenserCurrentPosition = 0;

bool cupPresence [ CUP_POSITIONS ] = { false };

void setupDispenserController() {
  
  pinMode ( END_SWITCH_PIN, INPUT );
  pinMode ( HOME_SWITCH_PIN, INPUT );
  
  pinMode ( DISPENSER_DIRECTION_PIN, OUTPUT );
  pinMode ( DISPENSER_STEP_PIN, OUTPUT );
  
  for ( int i = 0; i < CUP_POSITIONS; i ++ ) {
    pinMode ( cupDetectionPins [ i ], INPUT );
    attachInterrupt ( cupDetectionPins [ i ], cupDetectionSwitchChanged, CHANGE );
  }

  goToHome ();
  goToEnd ();
  moveDispenserHead ( maximumNumberOfSteps / 2, DIR_TO_HOME );

  for ( int i = 0; i < CUP_POSITIONS; i ++ ) {
    showCupStatusBlinky ( i, cupPresence [ i ] );
  }
}

void cupDetectionSwitchChanged () {
  
  for ( int i = 0; i < CUP_POSITIONS; i ++ ) {
    cupPresence [ i ] = ( digitalRead ( cupDetectionPins [ i ] ) == LOW );
  }
  
  for ( int i = 0; i < CUP_POSITIONS; i ++ ) {
    showCupStatusBlinky ( i, cupPresence [ i ] );
  }
  
//  #if defined( SOFTWARE_DEBUG )
//    Serial.print ( "Cup Detection: [" );
//    for ( int j = 0; j < CUP_POSITIONS; j ++ ) {
//      Serial.print ( cupPresence [ j ] );
//    }
//    Serial.println ( "]" );
//  #endif
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

bool inMotion = false;
bool goToCupPosition ( unsigned int cupPos ) {

  if ( cupPos >= 0 && cupPos <= CUP_POSITIONS ) {
    if ( !inMotion ) {
      inMotion = true;
      goToHome ();

      unsigned int stepsTaken = moveDispenserHead ( cupSteps [ cupPos ], DIR_TO_END );
    
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
      
      inMotion = false;
      return true;
    }
  }
  
  return false;
}

int getDispenserPositionAtCup () {
  int returnValue = -1;
  for ( int i = 0; i < CUP_POSITIONS; i ++ ) {
    int difference = dispenserCurrentPosition - cupSteps [ i ];
    if ( abs ( difference ) <= 2 ) {
      returnValue = i;
      break;
    }
  }
  return returnValue;
}

bool isCupDetected ( unsigned int cupId ) {
  return ( digitalRead ( cupDetectionPins [ cupId ] ) == LOW );
}

