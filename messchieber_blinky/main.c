/*
 * main.c
 *blinks on flank of caliper
 *
 *  Created on: 03.10.2015
 *      Author: db2442s
 */

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

uint8_t  mssclk;
uint8_t  mssclklast;


int main (void)
{

	DDRB = 0Xff; 		// sets Port B as input (ff = 11111111)
	DDRC = 0Xff; 		// sets Port C as input (ff = 11111111)
	PORTB = 0x03; 		//  sets pin 0 and 1 as "1" (03 = 11)
	DDRD = 0xff;
	DDRD |= (1 << PD6);

	mssclk = 0;
	mssclklast = 0;

	while(1)
	{
		mssclk = PINC & (1<<PC4);  // Signal sclk einlesen
		if (    mssclk    != 0 && mssclklast == 0    )  // pos Flanke
		{
			PORTD |= (1 << PD6);
			_delay_ms(1000);
			PORTD &= ~(1 << PD6);
		}
	}
	return 0;
}
