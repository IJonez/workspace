#ifndef _RS232_H_
#define _RS232_H_

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "GlobalConstants.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////// RS232 Definitions //////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// RS232_BAUDRATE, RS232_TX_BUFFER_SIZE, RS232_RX_BUFFER_SIZE müssen in GlobalConstants.h definiert werden
// z.B.
// #define RS232_BAUDRATE       19200
// #define RS232_TX_BUFFER_SIZE 32
// #define RS232_RX_BUFFER_SIZE 32
// 1,2,4,8,16,32,64,128 oder 256 Byte sind mögliche Puffergrößen
// Ist die Puffergröße = 0 wird der entsprechende Teil nicht aktiviert.
//
// Ist einer der Puffergrößen 0, wird der entsprechende Teil nicht angelegt
// 
// Eine Standard-PC-Schnittstelle verkraftet max. 57600 Baud
//
// Außerdem muss F_CPU festgelegt sein.
//
// Im Hauptprogramm müssen die Interrupts eingeschaltet werden!


#if !(defined(RS232_TX_BUFFER_SIZE) && defined(RS232_RX_BUFFER_SIZE))
   #error RS232_TX_BUFFER_SIZE und RS232_RX_BUFFER_SIZE müssen in GlobalConstants.h definiert werden
#endif 

//------------------------------------------------------
//--- Sendeteil ----------------------------------------
//------------------------------------------------------
#if RS232_TX_BUFFER_SIZE > 0
   void RS232Xmit(uint8_t data);         // Stellt ein Byte in den Übertragungspuffer. Wartet, wenn der Puffer voll ist.
   void RS232XmitHex(uint8_t data);      // Sendet ein Byte als zwei ASCII-Zeichen.
   void RS232XmitCrLf(void);             // Sendet ein Cr und ein Lf (carriage return, line feed).
   void RS232XmitStr(const char *data);  // Sendet einen nullterminierten String.

   // Sendet einen nullterminierten String aus dem Programmsegment.
   // const char foo[] PROGMEM = "Foo";
   // RS232XmitStr_P(foo);
   void RS232XmitStr_P(const char *data);

   // Sendet einen nullterminierten konstanten String aus dem Programmsegment.
   // RS232XmitTxt("Hello World");
   #define RS232XmitTxt(Txt) {RS232XmitStr_P(PSTR(Txt));} // dito

   // Wartet, bis die Übertragung fertig ist.
   static inline void RS232_flush()
   { while (UCSRB & (1 << UDRIE));
   }

void ClearRS232TxBuffer(void);        // Löscht den Übertragungspuffer.
#endif

//------------------------------------------------------
//--- Empfangsteil -------------------------------------
//------------------------------------------------------
#ifdef RS232_RX_BUFFER_SIZE
   // Liefert das nächste empfangene Zeichen aus der Eingabe-FIFO bzw. wartet darauf, falls die FIFO leer ist.
   uint8_t RS232_getc_wait();
 
   // Schaut nach, ob ein Zeichen empfangen wurde und liefert dieses gegebenenfalls als int zurück (Wertebereich ist 0...255). 
   // Wurde nichts empfangen, wird -1 geliefert.
   int RS232_getc_nowait(); // nicht impelementiert
   void ClearRS232RxBuffer(void);        // Löscht den Empfangsspuffer.
#endif

// Initialisiert den RS232 und aktiviert Receiver und Transmitter sowie den Receive-Interrupt gemäß Vorgabe.
// Die Ein- und Ausgabe-FIFO werden initialisiert. Das globale Interrupt-Enable-Flag (I-Bit in SREG) wird nicht verändert. 
// Nach dem Init ist eine kurze Pause notwenig. Irgendwie dauert es etwas, bis der PC das Startbit erkennt.
extern void RS232Init();

#endif /* _RS232_H_ */
