#include <avr/io.h>
#include <avr/interrupt.h>
#include "GlobalConstants.h"
#include "icp.h"


#if defined (__AVR_ATtiny2313__)
  // Definitionen für den ICP-PIN
  #define ICP_PIN		PIND			// ICP1 GPIO value
  #define ICP_PORT	   PORTD			// ICP1 GPIO port
  #define ICP_DDR		DDRD			// ICP1 GPIO DDR
  #define ICP_BIT		PD6			// ICP1 GPIO pin

  // Definitionen für den ICP timer (1).
  #define ICP_OCR		OCR1A			// ICP1 Output Compare register
  #define ICP_OC_IE	OCIE1A		// ICP1 timer Output Compare enable
  #define ICP_OC_IF	OCF1A			// ICP1 timer Output Compare flag
  #define ICP_IE		ICIE1			// ICP1 interrupt enable
  #define ICP_IF		ICF1			// ICP1 interrupt flag
  #define ICP_CTL_A	TCCR1A		// ICP1 timer control
  #define ICP_CTL		TCCR1B		// ICP1 interrupt control
  #define ICP_SENSE	ICES1			// ICP1 interrupt sense (rising/falling) 
  #define ICP_PRESCALE ((0 << CS12) | (1 << CS11) | (0 << CS10))	// prescale /8
  #define ICP_ICR     ICR1        // ICP1 Input Capture Register

  #define ICP_TIMSK   TIMSK       // Timer Interrupt Mask 
  #define ICP_INTFLG  TIFR        // Timer Interrupt Flags
  #define ICP_ICF     ICF1        // Input Capture Flag
  #define ICP_CMPF    OCF1A       // Output Compare A Match Flag

  //ISR-Vectoren
  #define  TIMER1_COMPARE_A_vector TIMER1_COMPA_vect

#elif defined (__AVR_ATtiny44__)
  // Definitionen für den ICP-PIN
  #define ICP_PIN		PINA			// ICP1 GPIO value
  #define ICP_PORT	PORTA			// ICP1 GPIO port
  #define ICP_DDR		DDRA			// ICP1 GPIO DDR
  #define ICP_BIT		PA7			// ICP1 GPIO pin

  // Definitionen für den ICP timer (1).
  #define ICP_OCR		OCR1A			// ICP1 Output Compare register
  #define ICP_OC_IE	OCIE1A		// ICP1 timer Output Compare enable
  #define ICP_OC_IF	OCF1A			// ICP1 timer Output Compare flag
  #define ICP_IE		ICIE1			// ICP1 interrupt enable
  #define ICP_IF		ICF1			// ICP1 interrupt flag
  #define ICP_CTL_A	TCCR1A		// ICP1 timer control
  #define ICP_CTL		TCCR1B		// ICP1 interrupt control
  #define ICP_SENSE	ICES1			// ICP1 interrupt sense (rising/falling) 
  #define ICP_PRESCALE ((0 << CS12) | (1 << CS11) | (0 << CS10))	// prescale /8
  #define ICP_ICR     ICR1        // ICP1 Input Capture Register

  #define ICP_TIMSK   TIMSK1      // Timer Interrupt Mask 
  #define ICP_INTFLG  TIFR1       // Timer Interrupt Flags
  #define ICP_ICF     ICF1        // Input Capture Flag
  #define ICP_CMPF    OCF1A       // Output Compare A Match Flag
  
  //ISR-Vectoren
  #define  TIMER1_COMPARE_A_vector TIM1_COMPA_vect

#else
  #error "device type not defined"
#endif


typedef	unsigned int icp_sample_t;

icp_sample_t icpStartTime;
icp_sample_t icpPeriod;
uint32_t     Input;  //Zwischenpuffer zum durchschieben der Bits
uint8_t      InpCnt; //Anzahl eingelesener Bits


ISR(TIMER1_CAPT_vect)
{ uint8_t      t;
  uint8_t      tmp = CaliperPIN;                // Datenleitung sichern
  icp_sample_t icr = ICP_ICR;		               // Timerstand sichern	

  // hier kann der Interrupt unterbrochen werden
  sei();

  icp_sample_t icpPeriod = icr - icpStartTime;  // Pulslänge berechnen
  icpStartTime = icr;				               // Neue Startzeit merken 

  if (tmp & CaliperDataMask)                    // Invertierung aufheben, CaliperDataMask maskiert den Data-PIN
  { t = 0;
  }
  else
  { t = 1;
  }


  if (icpPeriod < 0x1600)                // kurze oder lange Pause? 0x1600 entpricht bei 16MHz etwa 2,5 mSec
  { Input = (Input >> 1) | ((uint32_t) t << 23); //alten Wert rechtsschieben, neues Bit einfügen (auf Position 24)
    InpCnt++;                     // Bitzähler hochzählen
    if (InpCnt == 24)             // Alle Bits? Dann Ergebnis übertragen
    { cli();                      // während der Datenübergabe keine Unterbrechung zulassen
      CaliperValue = Input & 0xFFFFF;
      if (Input & 0x100000)
      { CaliperValue = -CaliperValue;
      }
      if (Input & 0x800000)
      { CaliperUnit = 2; // inch
      }
      else
      { CaliperUnit = 1; // mm
      }
      sei();                      // Datenübergabe vollständig
    }
  }
  else  // Neues Datenpaket
  { Input = (uint32_t) t << 23;    // Zwischenspeicher initialisieren: 1. neue Bit auf Position 24
    InpCnt = 1;                    // Bitzähler auf 1
  }
  return;
}

// Timer und Flags für den ICP vorbereiten
void icpInit(void)
{ ICP_CTL	= (0 << ICP_SENSE) | ICP_PRESCALE; //fallende Flanke und Prescaler


  ICP_INTFLG = (1<<ICP_ICF); //evtl. eingetretene Interupts löschen

  ICP_TIMSK	|= (1<<ICP_IE); //Input Capture Interrupt zulassen

  return;
}
