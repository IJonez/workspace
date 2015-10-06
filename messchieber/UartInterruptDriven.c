#ifndef UARTINTERRUPTDRIVE_H
#define UARTINTERRUPTDRIVE_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "GlobalConstants.h"
#include "UartInterruptDriven.h"


// Einstellungen prüfen 
#if !(defined(RS232_BAUDRATE) && defined(RS232_TX_BUFFER_SIZE) && defined(RS232_RX_BUFFER_SIZE))
   #error RS232_BAUDRATE, RS232_TX_BUFFER_SIZE und RS232_RX_BUFFER_SIZE müssen in GlobalConstants.h definiert werden
#endif 


#ifndef F_CPU
   #error F_CPU nicht festgelegt
#endif

#if RS232_TX_BUFFER_SIZE > 0
   #define RS232_TX_BUFFER_MASK (RS232_TX_BUFFER_SIZE - 1)
   #if (RS232_TX_BUFFER_SIZE & RS232_TX_BUFFER_MASK)
      #error RS232 TX Puffergröße ist keine Potenz von 2
   #endif
   uint8_t RS232_TxBuf[RS232_TX_BUFFER_SIZE];
   volatile uint8_t RS232_TxHead = 0;
   volatile uint8_t RS232_TxTail = 0;
#endif

#if RS232_RX_BUFFER_SIZE > 0
   #define RS232_RX_BUFFER_MASK (RS232_RX_BUFFER_SIZE - 1)
   #if (RS232_RX_BUFFER_SIZE & RS232_RX_BUFFER_MASK)
      #error RS232 RX Puffergröße ist keine Potenz von 2
   #endif
   uint8_t RS232_RxBuf[RS232_RX_BUFFER_SIZE];
   volatile uint8_t RS232_RxHead = 0;
   volatile uint8_t RS232_RxTail = 0;
#endif


void RS232Init()
{ uint8_t sreg = SREG;
  uint16_t ubrr = (uint16_t) ((uint32_t) F_CPU/(16* (uint32_t) RS232_BAUDRATE) - 1);

  UBRRH = (uint8_t) (ubrr>>8);
  UBRRL = (uint8_t) (ubrr);


  // Interrupts kurz deaktivieren 
  cli();

#if RS232_TX_BUFFER_SIZE > 0
  #define TX_ENABLE (1<<TXEN)
#else
  #define TX_ENABLE 0
#endif

#if RS232_RX_BUFFER_SIZE > 0
  #define RX_ENABLE ((1<<RXEN) | (1<<RXCIE))
#else
  #define RX_ENABLE 0
#endif

  // RS232 Receiver und Transmitter anschalten, Receive-Interrupt aktivieren 
  // Data mode 8N1, asynchron

  UCSRB = TX_ENABLE | RX_ENABLE;
#if defined (__AVR_ATmega8__)
  UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
#elif defined (__AVR_ATtiny2313__)
  UCSRC = (1 << UCSZ1) | (1 << UCSZ0);
#else
  #error nicht konfigurierter CPU-Typ
#endif

  // Flush Receive-Buffer (entfernen evtl. vorhandener ungültiger Werte) 
  do
    { // UDR auslesen (Wert wird nicht verwendet) 
      UDR;
    }
    while (UCSRA & (1 << RXC));

  // Rücksetzen von Receive und Transmit Complete-Flags 
  UCSRA = (1 << RXC) | (1 << TXC);

  // Global Interrupt-Flag wieder herstellen 
  SREG = sreg;

  // FIFOs für Ein- und Ausgabe initialisieren 
//  ClearRS232TxBuffer();
//  ClearRS232RxBuffer();
}


//--------------------------------------------------
//--- Sendeteil ------------------------------------
//--------------------------------------------------
#if RS232_TX_BUFFER_SIZE > 0

// Stellt ein Byte in den Übertragungspuffer.
// Wartet, wenn der Puffer voll ist.
void RS232Xmit(uint8_t data)
{ unsigned char tmphead;

  tmphead = (RS232_TxHead + 1) & RS232_TX_BUFFER_MASK;       // Puffer-Index berechnen
  while ( tmphead == RS232_TxTail );                         // Warten bis freie Speicherplätze vorhanden sind.
  RS232_TxBuf[tmphead] = data;                               // Datenbyte speichern.
  RS232_TxHead = tmphead;                                    // Index speichern
  UCSRB |= (1 << UDRIE);                                     // Interrupt freigeben  
}


void XmitNibble(uint8_t n) //Private Funktion
{ uint8_t d = n + 0x30;
  if (d > 0x39)
     d += 7;
  RS232Xmit(d);
}

void RS232XmitHex(uint8_t data)
{ XmitNibble(data >> 4);
  XmitNibble(data & 0x0F);
}


void RS232XmitCrLf(void)
{ RS232Xmit(13); 
  RS232Xmit(10);
}

void RS232XmitStr(const char *data)
{ while (*data)
    RS232Xmit(*data++);
}

void RS232XmitStr_P(const char *data)
{ while (pgm_read_byte(data) != 0x00)
    RS232Xmit(pgm_read_byte(data++));
}

void ClearRS232TxBuffer(void)
{ RS232_TxTail = 0;
  RS232_TxHead = 0;
}

// Ein Zeichen aus der Ausgabe-FIFO lesen und ausgeben 
// Ist das Zeichen fertig ausgegeben, wird ein neuer SIG_RS232_DATA-IRQ getriggert 
// Ist die FIFO leer, deaktiviert die ISR ihren eigenen IRQ. 
SIGNAL (SIG_USART0_DATA)
{ if (RS232_TxHead != RS232_TxTail)       // Es sind Daten vorhanden
  { RS232_TxTail = ( RS232_TxTail + 1 ) & RS232_TX_BUFFER_MASK;
    UDR = RS232_TxBuf[RS232_TxTail];
  }
  else
    UCSRB &= ~(1 << UDRIE);
}

#endif //Sendeteil



//--------------------------------------------------
//--- Empfangsteil ---------------------------------
//--------------------------------------------------
#if RS232_RX_BUFFER_SIZE > 0

void ClearRS232RxBuffer(void)
{ RS232_RxTail = 0;
  RS232_RxHead = 0;
}

// Empfangene Zeichen werden in die Eingabgs-FIFO gespeichert und warten dort 
SIGNAL (SIG_USART0_RX)
{
#error Empfang noch nicht implementiert
  //  _inline_fifo_put (&infifo, UDR);
}

#endif //Empfangsteil


#endif //UARTINTERRUPTDRIVE_H
