#include <ArduinoJson.h>

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

/*
 * JSON Structure for Barvis Commands
 * {
 *  "type" : { "AT" | "PUMP" | "SET" },
 *  "at_cmd" : "<ATCMD>",
 *  "run_pumps" : [ { "pump_id" : <pumpID>, "run_for" : <runForUnits> }, ...  ]
 *  "set" : [ { "key" : "value" }, { "key2" : "value2" } ... ]
 * }
 *
 */

StaticJsonBuffer<1024> jsonBuffer;
#define WAIT_DELAY_FOR_CUP_DETECTION  250

PumpOperation pumpOperations [ PUMP_COUNT ];

void loop() {

  String commandJson = "";
  if ( bleSerial.available() ) {
    commandJson = bleSerial.readString ();
  }
  else if (Serial.available())  {
    commandJson = Serial.readString();
  }

  if ( commandJson != "" ) {

#if defined( SOFTWARE_DEBUG )
    Serial.println ( commandJson );
#endif

    JsonObject & jsonObject = jsonBuffer.parseObject(commandJson);

#if defined( SOFTWARE_DEBUG )
    Serial.println ( "After the parsing line" );
#endif

    if (jsonObject.success()) {

#if defined( SOFTWARE_DEBUG )
    Serial.println ( "JSON parsing is successfull" );
#endif
      String cmdType = jsonObject [ "type" ];

      if ( cmdType != "" ) {

#if defined( SOFTWARE_DEBUG )
    Serial.println ( "CMD Type: " + cmdType );
#endif
        if ( cmdType == "PUMP" ) {

#if defined( SOFTWARE_DEBUG )
    Serial.println ( "Getting Pump Data" );
#endif
          JsonArray& runPumpsArray = jsonObject [ "run_pumps" ].asArray();
          int cmdLength = runPumpsArray.size();

#if defined( SOFTWARE_DEBUG )
    Serial.print  ( "Total Pumps command Size is: " );
    Serial.println ( cmdLength );
#endif
          int i = 0;
          for ( JsonArray::iterator it=runPumpsArray.begin(); it!=runPumpsArray.end(); ++it) {
            JsonObject & pumpConfig = it -> asObject ();
            byte id = (byte) pumpConfig [ "pump_id" ];
            byte duration = (byte) pumpConfig [ "run_for" ];
#if defined( SOFTWARE_DEBUG )
    Serial.print  ( "Pump ID: " );
    Serial.print ( id );
    Serial.print ( ", Duration: " );
    Serial.println ( duration );
#endif
            pumpOperations [ i ].pumpId = id;
            pumpOperations [ i ].runForSeconds = duration;
            i ++;
          }
//          processCommand ( "" );
            runPumpsFor ( pumpOperations, cmdLength );
        }
        else if ( cmdType == "SET" ) {
          //TODO: // ChangeSettings method here
        }
        else if ( cmdType == "AT" ) {
          String atCommand = jsonObject [ "at_cmd" ];
          sendAtCommand ( atCommand );
        }
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
