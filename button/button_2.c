#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

int main(void)
	{
	DDRD = 0b11111110; 		//

	while(1)
		{
		if (PIND & (1<<PD7))  //Button gedrückt
		{
			PORTD |= (1<<PD6); // LED an
			_delay_ms(1000);    // LED immer noch an
			PORTD &= ~(1 << PD6);  // LED aus
		}

		}
	return 0;
	}
