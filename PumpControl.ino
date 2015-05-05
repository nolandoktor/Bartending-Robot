#define PUMP_1  0
#define PUMP_2  1
#define PUMP_3  2
#define PUMP_4  3
#define PUMP_5  4
#define PUMP_6  5

#define PUMP_COUNT 6
int pumpPins [ PUMP_COUNT ] = { 2, 3, 4, 5, 6, 7 };
int pumpRunningTime [ PUMP_COUNT ] = { 0 };

IntervalTimer botTimer;

void setupPumpConroller () {
  for ( int i = 0; i < PUMP_COUNT; i ++) {
    pinMode(pumpPins [ i ], OUTPUT);
  }
  
  botTimer.begin(OneSecondTimer, 1000000);

  Serial.begin(9600);
}

void runPump ( int pumpID, int timeInSecs ) {
  
  if ( timeInSecs > 0 ) {
    digitalWrite ( pumpPins [ pumpID ], LOW );
    pumpRunningTime [ pumpID ] = timeInSecs;
  }
}

void turnOffPump ( int pumpID ) {
  digitalWrite ( pumpPins [ pumpID ], HIGH );
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

void pourTime(int pump1, int pump2, int pump3, int pump4, int pump5, int pump6) {

  runPump ( PUMP_1, pump1 );
  runPump ( PUMP_2, pump2 );
  runPump ( PUMP_3, pump3 );
  runPump ( PUMP_4, pump4 );
  runPump ( PUMP_5, pump5 );
  runPump ( PUMP_6, pump6 );
  
}

