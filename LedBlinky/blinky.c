/*
 * blinky.c
 *
 *  Created on: 10.09.2015
 *      Author: db2442s
 */


#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>


int main(void){
	DDRD |= (1 << PD6);
	while(1)
	{
		PORTD |= (1 << PD6);
		_delay_ms(1000);
		PORTD &= ~(1 << PD6);
		_delay_ms(100);

	}

	return 0;
}
