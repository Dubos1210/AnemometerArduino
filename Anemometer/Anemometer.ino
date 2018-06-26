#define F_CPU 16000000UL //16MHz

#define PLOTTER	0        //Âûâîä â ôîðìàòå ïðîãðàììû SerialPortPlotter

#define rot 2300         //Ïóòü âîçäóõà çà îäèí îáîðîò, ìì * 10

#include <avr/io.h>\
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "usart.h"

unsigned int windspeed = 0;
unsigned int counter = 0;

ISR(TIMER0_COMPA_vect) {
	counter++;
	TCNT0 = 0;
}

ISR(INT1_vect) {
	EIMSK &=~ (1<<INT0);
	
	PORTB |= (1<<5);
	if(counter > 0) {
		windspeed = rot / (counter * 2);
		counter = 0;
	}
	
		#if PLOTTER
			USART_send(36);
			USART_send_integer((int) windspeed / 10);
			USART_send(0x2E);
			USART_send_integer((int) windspeed % 10);
			USART_send_string(";");
			USART_send_BK();
		#else
		if(windspeed >= 0.001) {
			USART_send_string("Wind speed: ");
			USART_send_integer((int) windspeed / 10);
			USART_send(0x2E);
			USART_send_integer((int) windspeed % 10);
			USART_send_string(" m/s");
			USART_send_BK();
		}
		#endif
	
	_delay_ms(100);	
	PORTB &=~ (1<<5);
	
	EIMSK = 2;
	EIFR |= (1<<1);
}

int main(void) {
	DDRB |= (1<<5); //LED íà 13 âûâîäå
	PORTB |= (1<<5);
		
	//USART
	DDRD &=~ (1<<0);   //PD0 (RX) - âûõîä
	DDRD |= (1<<1);    //PD1 (TX) - âõîä
	USART_init(38400);
	#if PLOTTER		
	#else
		USART_send_string("Dubos Anemometer");
		USART_send_BK();
	#endif
	
	//Magnet
	DDRD &=~ (1<<3); //INT1 íà 3 íîãå
	PORTD |= (1<<3); //Pullup íà INT1 íîãå
	DDRD |= (1<<4);  //Çåìëÿ íà 4 íîãå
	PORTD &=~ (1<<4);
	
	EICRA = 8;
	EIMSK = 2;
		
	//For ATmega328
	TCCR0A = 0;
	TCCR0B = 3;
	TIMSK0 = 2;
	OCR0A = 125;
		
	sei();
	
	_delay_ms(500);
	PORTB &=~ (1<<5);
	
	while(1) {		
		
	}
	
	//while(1) {}
}
