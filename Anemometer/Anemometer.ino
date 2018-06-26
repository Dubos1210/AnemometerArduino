#define F_CPU 16000000UL //16MHz

#define PLOTTER	0        //Вывод в формате программы SerialPortPlotter

#define rot 2300         //Путь воздуха за один оборот, мм * 10

#define DSOK_flag 0x01

#include <avr/io.h>\
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "DS18B20.h"

int DS_temperature = 0;
unsigned int windspeed = 0;
unsigned int counter = 0;
unsigned char flag = 0;

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
			if(flag & DSOK_flag) {
				DS_temperature = DS18B20_temperature();
				USART_send_decimal(DS_temperature, 1);
				USART_send(" ");
			}
			USART_send_integer((int) windspeed / 10);
			USART_send(0x2E);
			USART_send_integer((int) windspeed % 10);
			USART_send_string(";");
			USART_send_BK();
		#else
		if(windspeed >= 0.001) {
			if(flag & DSOK_flag) {
				DS_temperature = DS18B20_temperature();
				USART_send_string("Temperature: ");
				USART_send_decimal(DS_temperature, 1);
				USART_send_string(" C");
				USART_send_BK();
			}
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
	DDRB |= (1<<5); //LED на 13 выводе
	PORTB |= (1<<5);
		
	//USART
	DDRD &=~ (1<<0);   //PD0 (RX) - выход
	DDRD |= (1<<1);    //PD1 (TX) - вход
	USART_init(38400);
	#if PLOTTER		
	#else
		USART_send_string("Dubos Anemometer");
		USART_send_BK();
	#endif
	
	//Magnet
	DDRD &=~ (1<<3); //INT1 на 3 ноге
	PORTD |= (1<<3); //Pullup на INT1 ноге
	DDRD |= (1<<4);  //Земля на 4 ноге
	PORTD &=~ (1<<4);
	
	EICRA = 8;
	EIMSK = 2;
	
	
	/*//DS18B20
	#if PLOTTER
	#else
		USART_send_string("Seraching for DS18B20 (Library v0.1 by Dubos)");
	#endif	
	if(DS18B20_init() > 0) {
		#if PLOTTER
		#else
			USART_send_string(" - Found");
			USART_send_BK();
		#endif	
		DS18B20_write(SKIP_ROM);
		DS18B20_write(WRITE_SCRATCHPAD);
		DS18B20_write(0x7F);
		DS18B20_write(0x7F);
		DS18B20_write(0x7F);
		
		flag |= (1<<DSOK_flag);
	}
	else {
		#if PLOTTER
		#else
			USART_send_string(" - Not found");
			USART_send_BK();
		#endif
		flag &=~ (1<<DSOK_flag);
	}*/	
	
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