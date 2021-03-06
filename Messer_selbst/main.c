#include <avr/io.h>
#include <stdint.h>

uint32_t ms_data;
uint32_t ms_data_out;
uint8_t  ms_sclk;
uint8_t  ms_sclk_last;
uint8_t i;
uint64_t tick;
uint64_t time_last_flank;
float time;



int main(void)
	{

	sei();

	void init_TIMER()
	{
		TCCR0 = (1<<CS02) | (1<<CS00); //prescaler is 1024
		TIMSK |= (1<<TOIE0);  //overflow interrupt is enabled
	}

	time init_tictac()
	{
		return(tick * 0.128 + TCNT0 * 0.0005);
	}

	DDRB = 0Xff; 		// sets Port B as input (ff = 11111111)
	PORTB = 0x03; 		//  sets pin 0 and 1 as "1" (03 = 11)
	DDRD = 0xff;

	while(1)
		{
		ms_sclk = PIND & (1<<PD7);  // Signal sclk einlesen

		if (    ms_sclk    != 0 && ms_sclk_last == 0    )  // pos Flanke
		{
			if (time() - time_last_flank <= 100)       //max. time between data sets in ms
			{
				ms_data += (PIND & (1<<PD6));
				i += 1;
				time_last_flank = time();
			}
			else
			{
				ms_data = 000000000000000000000000;
			}

		}

		ms_sclk_last = ms_sclk;

		if (i == 24)
		{
			ms_data_out = ms_data;
			i = 0;
		}
		}
	return 0;
	}

ISR( TIMER0_OVF_vect ) //timer isr
	{
		tick += 1;    //for every overflow 0.128 ms
	}

