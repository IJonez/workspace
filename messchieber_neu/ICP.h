#ifndef ICP_H
#define ICP_H

volatile int32_t CaliperValue; // enth�lt den letzten eingelesen Messwert
volatile uint8_t CaliperUnit;  // zugeh�rige Einheit 1=mm, 2=inch, 0=kein g�ltiger Messwert

void icpInit();

#endif	// ICP_H
