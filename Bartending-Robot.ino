#include <SoftwareSerial.h>

// comment the following define statement to shut the built-in LED off
#define USE_DEBUG_LED      1

// Comment the following to skip Serial Port Setup
#define SOFTWARE_DEBUG     1

#define PUMP_COUNT         6
#define ORDER_QUEUE_SIZE   4
#define CUP_POSITIONS      ORDER_QUEUE_SIZE

// ------------- PUMP Control Functions Start ---------------- //
struct PumpOperation {
  byte pumpId;
  byte runForSeconds;
};

void setupPumpConroller ();
void showPumpStatus ();
bool isPumpRunning ();
unsigned int getCurrentPumpStatus ();
void runPumpsFor ( PumpOperation * pumpOps, int count );
// -------------  PUMP Control Functions End  ---------------- //

// ------------   BLE Control Functions Start ---------------- //
const int bleRxPin = 0;
const int bleTxPin = 1;
SoftwareSerial bleSerial ( bleRxPin, bleTxPin ); // RX, TX

void setupBleControl();
void sendAtCommand ( String cmd );
// ------------   BLE Control Functions End   ---------------- //

// ----------- Order Control Functions Start ----------------- //
void setupOrderController ();
const int * getNextOrder ();
void orderProcessed ();
int getOrderQueSize ();
int * getOrderQueStatus ( int * statusArray, int maxQueSize );
// -----------  Order Control Functions End  ----------------- //


// ----------- Dispenser Control Functions Start ------------- //
void setupDispenserController ();
bool goToCupPosition ( unsigned int );
bool isCupDetected ( unsigned int );
int getDispenserPositionAtCup ();
// -----------  Dispenser Control Functions End  ------------- //


// ------------- Blinky Control Functions Start -------------- //
void setupBlinkyController ();
// -------------  Blinky Control Functions End  -------------- //


#if defined( USE_DEBUG_LED )
IntervalTimer debugLedTimer;
#endif

void setup() {

  // Debug LED setup
#if defined( USE_DEBUG_LED )
  pinMode ( LED_BUILTIN, OUTPUT );
  debugLedTimer.begin ( debugLedTimerRoutine, 250000 );
#endif

  Serial.begin(9600);
  delay(2000);// Give reader a chance to see the output.

  setupPumpConroller ();
  setupOrderController ();
  setupDispenserController ();
  setupBlinkyController ();
}

#if defined( USE_DEBUG_LED )
bool builtInLedState = HIGH;
unsigned int previousPumpState = 0xFFFF;

void debugLedTimerRoutine () {
  builtInLedState = !builtInLedState;
  digitalWrite ( LED_BUILTIN, builtInLedState );

#if defined( SOFTWARE_DEBUG )
  unsigned int currStatus = getCurrentPumpStatus ();
  if ( previousPumpState != currStatus ) {
    Serial.print ( "Current pump satus: " );
    Serial.println ( currStatus );
  }
  previousPumpState = currStatus;
#endif
}
#endif

int currCupPosition = 0;
void incrementCupPoistion () {
  currCupPosition = ( currCupPosition + 1 ) % CUP_POSITIONS;
}

int orderQueStatus [ CUP_POSITIONS ] = { -1 };
void showOrderStatusBlinky () {
  getOrderQueStatus ( orderQueStatus, CUP_POSITIONS );
  showOrderQueue ( currCupPosition, CUP_POSITIONS, orderQueStatus );
}

void showCupStatusBlinky ( int cupId, bool isPresent ) {
  showCupStatus ( cupId, isPresent );
}

void processCommand ( String cmd ) {

#if defined( SOFTWARE_DEBUG )
  Serial.print ( "Processing: " );
  Serial.println ( cmd );
#endif

//TODO: Add barvis control logic here
//  if ( cmd.startsWith ( "BLE:" ) ) {
//    String blinkyCmd = cmd.substring ( 4 );
//    setBlinky ( !blinkyCmd.compareTo ( "ON" ) );
//  }
}

#define WAIT_DELAY_FOR_CUP_DETECTION  250
void loop() {
    
  if ( bleSerial.available() ) {
    String bleCommand = bleSerial.readString ();
    Serial.println ( bleCommand );
    processCommand ( bleCommand );
  }
  else {
    if (Serial.available())  {
      String serialCommand = Serial.readString();
      if ( serialCommand.startsWith ( "AT" ) ) {
        sendAtCommand ( serialCommand );
      }
      else {
        processCommand ( serialCommand );
      }
    }
  }


  //  if ( !isPumpRunning () ) {
  //
  //    const int * recipie = getNextOrder ();
  //
  //    if ( recipie != NULL ) {
  //
  //      int currentDispenserHeadAt = getDispenserPositionAtCup ();
  //
  //      if ( currentDispenserHeadAt != currCupPosition ) {
  //        goToCupPosition ( currCupPosition );
  //      }
  //
  //      while ( !isCupDetected ( currCupPosition ) ) { // wait for the cup to be placed
  //        delay ( WAIT_DELAY_FOR_CUP_DETECTION );
  //      }
  //
  //      if ( isCupDetected ( currCupPosition ) ) {
  //        runPumpsFor ( recipie );
  //        orderProcessed ();
  //
  //        incrementCupPoistion ();
  //      }
  //    }
  //  }
}


