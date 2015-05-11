#define PUMP_1  0
#define PUMP_2  1
#define PUMP_3  2
#define PUMP_4  3
#define PUMP_5  4
#define PUMP_6  5

int pumpPins [ PUMP_COUNT ] = { 18, 19, 20, 21, 22, 23 };
int pumpRunningTime [ PUMP_COUNT ] = { 0 };

IntervalTimer botTimer;

void setupPumpConroller () {

  for ( int i = 0; i < PUMP_COUNT; i ++) {
    pinMode ( pumpPins [ i ], OUTPUT );
    turnOffPump ( i );
  }  
}

volatile unsigned int pumpsStatus = 0;

void runPump ( int pumpID, int timeInSecs ) {
  
  if ( timeInSecs > 0 ) {
    digitalWrite ( pumpPins [ pumpID ], LOW );
    pumpRunningTime [ pumpID ] = timeInSecs;
    pumpsStatus |= ( 0x1 << pumpID ); 
  }

  showPumpStatus ( isPumpRunning () );
}

void turnOffPump ( int pumpID ) {
  digitalWrite ( pumpPins [ pumpID ], HIGH );
  pumpsStatus &= ~( 0x1 << pumpID ); 
  showPumpStatus ( isPumpRunning () );
}

bool isPumpRunning () {
  return ( pumpsStatus != 0 );
}

unsigned int getCurrentPumpStatus () {
  return pumpsStatus;
}


void OneSecondTimer () {
  
  for ( int i = 0; i < PUMP_COUNT; i ++ ) {
    
    pumpRunningTime [ i ] -= 1;
    
    if ( pumpRunningTime [ i ] == 0 ) {
      turnOffPump ( i );
    }
    
    if ( pumpRunningTime [ i ] < 0 ) {
      pumpRunningTime [ i ] = 0;
    }
  }
}

int runningPumpsCount () {
  int returnValue = 0;
  
  for ( int i = 0; i < PUMP_COUNT; i ++ ) {
    if ( pumpRunningTime [ i ] > 0 ) {
      returnValue ++;
    }
  }
  
  return returnValue;
}

void runPumpsFor ( const int forSeconds [] ) {
  
  botTimer.end ();
  
  #if defined( SOFTWARE_DEBUG )
    Serial.print ( "Running Pumps for: " );
    for ( int i = 0; i < PUMP_COUNT; i ++ ) {
      Serial.print ( forSeconds [ i ] );
    }
    Serial.println ( "." );
  #endif
  
  for ( int i = 0; i < PUMP_COUNT; i ++ ) {
    runPump ( i, forSeconds [ i ] );
  }

  botTimer.begin ( OneSecondTimer, 1000000 );
}


