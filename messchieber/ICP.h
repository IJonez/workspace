#ifndef ICP_H
#define ICP_H

volatile int32_t CaliperValue; // enthält den letzten eingelesen Messwert
volatile uint8_t CaliperUnit;  // zugehörige Einheit 1=mm, 2=inch, 0=kein gültiger Messwert

void icpInit();

#endif	// ICP_H
