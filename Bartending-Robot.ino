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
  debugLedTimer.begin ( debugLedTimerRoutine, 500000 );
#endif

  Serial.begin(115200);
  delay(2000);// Give reader a chance to see the output.

  setupPumpConroller ();
  setupOrderController ();
  setupDispenserController ();
  setupBlinkyController ();
  setupBleControl ();
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

/*
   Command Structure
   CMD -> AT|PUMP|SET
   AT -> "AT":<ATCMD>
   PUMP -> "PUMP":PUMP_OP
   PUMP_OP -> <ID>=<TIME>,PUMP_OP*
   SET -> "SET":SETTING
   SETTING -> <KEY>=<VALUE>,SETTING*
   ----------------------------------
   JSON Structure for Barvis Commands
   {
    "type" : { "AT" | "PUMP" | "SET" },
    "at_cmd" : "<ATCMD>",
    "run_pumps" : [ { "pump_id" : <pumpID>, "run_for" : <runForUnits> }, ...  ]
    "set" : [ { "key" : "value" }, { "key2" : "value2" } ... ]
   }

*/

#define WAIT_DELAY_FOR_CUP_DETECTION  250

void loop() {

  String command = "";
  if ( bleSerial.available() ) {
    command = bleSerial.readString ();
  }
  else if (Serial.available())  {
    command = Serial.readString();
  }

  if ( command != "" ) {

#if defined( SOFTWARE_DEBUG )
    Serial.println ( command );
#endif

    // PUMP:1=2,2=3,3=4,4=5,5=6,6=7,
    
    int cmdIndex = command.indexOf ( ':' );
    String cmdType = command.substring ( 0, cmdIndex ).trim();

    if ( cmdType != "" ) {
      if ( cmdType == "PUMP" ) {
        
        int startIndex = cmdIndex;
        int endIndex = command.indexOf ( ',', startIndex );
        int totalCommands = 0;
        
        while ( endIndex != -1 ) {
          startIndex = endIndex + 1;
          endIndex = command.indexOf ( ',', startIndex );
          totalCommands ++;
        }

        PumpOperation * pumpOperations = new PumpOperation [ totalCommands ];
        startIndex = cmdIndex + 1;
        endIndex = command.indexOf ( ',', startIndex );
        int i = 0;
        while ( endIndex != -1 ) {
          String pumpOp = command.substring ( startIndex, endIndex );
          int opSeparator = pumpOp.indexOf ( '=' );
          int id = pumpOp.substring ( 0, opSeparator ).toInt();
          int duration = pumpOp.substring ( opSeparator + 1 ).toInt ();

#if defined( SOFTWARE_DEBUG )
            Serial.print  ( "Pump ID: " );
            Serial.print ( id );
            Serial.print ( ", Duration: " );
            Serial.println ( duration );
#endif

          pumpOperations [ i ].pumpId = id;
          pumpOperations [ i ].runForSeconds = duration;
          i ++;
          startIndex = endIndex + 1;
          endIndex = command.indexOf ( ',', startIndex );
        }
        
        runPumpsFor ( pumpOperations, totalCommands );
        delete pumpOperations;
      }
      else if ( cmdType == "SET" ) {
        //TODO: // ChangeSettings method here
      }
      else if ( cmdType == "AT" ) {
        String atCommand = command.substring ( cmdIndex + 1 );
        sendAtCommand ( atCommand );
      }
    }
    else {
#if defined( SOFTWARE_DEBUG )
      Serial.println("parseObject() failed");
#endif

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
}
