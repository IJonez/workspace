#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "GlobalConstants.h"
#include "UartInterruptDriven.h"
#include "ICP.h"

#define CR "\r\n"

char buffer[10];


int main(void)
{ RS232Init();  // serielle Schnittstelle initialisieren
  CaliperPORT |= CaliperClockMask | CaliperDataMask; //pull up einschalten
  icpInit();    // ICP initialisieren
  sei();        // Interrupts freigeben



  RS232XmitTxt("ATmega8 running" CR);

  while(1)
  { uint8_t tmpUnit;
    int32_t tmpValue;
  
    if (CaliperUnit)              // es liegen neue Daten vor
    { cli();                      // während der Datenübergabe keine Unterbrechung zulassen
      tmpUnit  = CaliperUnit;     // Messdaten zwischenspeichern
      tmpValue = CaliperValue;
      CaliperUnit = 0;            // Kennung auf gültigen Wert löschen
      sei();                      // Datenübergabe vollständig

      RS232XmitStr(ltoa(tmpValue, buffer, 10));
      if (tmpUnit == 2)
      { RS232XmitStr(" inch");
      }
      else
      { RS232XmitStr(" mm");
      }
      RS232XmitCrLf();
    }
  } // end while
}
