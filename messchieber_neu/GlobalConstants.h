#ifndef GLOBALCONSTANTS_H
#define GLOBALCONSTANTS_H

#define RS232_BAUDRATE       57600
#define RS232_TX_BUFFER_SIZE 64
#define RS232_RX_BUFFER_SIZE 0

/*
Pinbelegung:
PD2 (INT0): Clock (Kabelfarbe Orange)
PD3 (INT1): Data  (Kabelfarbe Gelb)
*/

#define CaliperPORT      PORTC
#define CaliperDDR       DDRC
#define CaliperPIN       PINC
#define CaliperClockMask _BV(PC4)
#define CaliperDataMask  _BV(PC3)

#endif
