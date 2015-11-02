#include <SoftwareSerial.h>

const int baudRate = 9600;

void setupBleControl()
{
  pinMode ( bleRxPin, INPUT );
  pinMode ( bleTxPin, OUTPUT );

#if defined( SOFTWARE_DEBUG )
  Serial.println ( "Serial Port initialized!" );
#endif

  bleSerial.begin ( baudRate );
  //  bleSerial.setTimeout ( 10000 );

  const int numberOfInitCommands = 7;
  String initializationCommands [ numberOfInitCommands ] = {
    "AT",
    "AT+NOTI0",
    "AT+ROLE0",
    "AT+RESET",
    "AT+SHOW1",
    "AT+IMME1",
    "AT+NAMEBARVIS"
  };
  // AT+DISC? is to discover the devices around HM11

  for ( int i = 0; i < numberOfInitCommands; i++ ) {
    sendAtCommand ( initializationCommands [ i ] );
  }
}

void sendAtCommand ( String cmd ) {

#if defined( SOFTWARE_DEBUG )
  Serial.println ( cmd );
#endif

  bleSerial.print ( cmd );
  bleSerial.flush ();

//  while ( !bleSerial.find ( `(char *) "OK" ) ) {
//#if defined( SOFTWARE_DEBUG )
//    Serial.print ( "." );
//#endif
//  }

#if defined( SOFTWARE_DEBUG )
//  Serial.println ( " .. OK" );
  Serial.println ( bleSerial.readString () );
#endif

}

