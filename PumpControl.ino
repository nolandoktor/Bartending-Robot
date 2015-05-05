#define USING_TEENSY_PLUS_PLUS_2_0  1
//#define USING_TEENSY_3_0  1

#define PUMP_1  0
#define PUMP_2  1
#define PUMP_3  2
#define PUMP_4  3
#define PUMP_5  4
#define PUMP_6  5

#define PUMP_COUNT 6
int pumpPins [ PUMP_COUNT ] = { 38, 39, 40, 41, 42, 43 };
int pumpRunningTime [ PUMP_COUNT ] = { 0 };

#if defined (USING_TEENSY_3_0)
  IntervalTimer botTimer;
#endif
#if defined ( USING_TEENSY_PLUS_PLUS_2_0 )
  #include <TimerOne.h>
#endif

void setupPumpConroller () {

  for ( int i = 0; i < PUMP_COUNT; i ++) {
    pinMode ( pumpPins [ i ], OUTPUT );
  }
  
#if defined (USING_TEENSY_3_0)
  botTimer.begin ( OneSecondTimer, 1000000 );
#endif

#if defined ( USING_TEENSY_PLUS_PLUS_2_0 )
  Timer1.initialize ( 1000000 );
#endif
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

void pourTime ( int pump1, int pump2, int pump3, int pump4, int pump5, int pump6 ) {

#if defined ( USING_TEENSY_PLUS_PLUS_2_0 )
  Timer1.stop();
  Timer1.detachInterrupt();
#endif

  runPump ( PUMP_1, pump1 );
  runPump ( PUMP_2, pump2 );
  runPump ( PUMP_3, pump3 );
  runPump ( PUMP_4, pump4 );
  runPump ( PUMP_5, pump5 );
  runPump ( PUMP_6, pump6 );
  
#if defined ( USING_TEENSY_PLUS_PLUS_2_0 )
  Timer1.attachInterrupt ( OneSecondTimer );
  Timer1.resume();
#endif

}

elapsedMillis millisSinceLastRun = 0;

void pumpControlMainRoutine () {
  
  if ( millisSinceLastRun > 10000  ) {
    pourTime ( 5, 2, 3, 4, 1, 6 );
    millisSinceLastRun = 0;
  }
}

