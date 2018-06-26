/* DS18B20 Library for Atmel AVR (v0.1)
 *  DS18B20_init();        //Reset-импульс (возвращает 0xFF, если датчик ответил)
 *  DS18B20_write(0x**);   //Отправка байта 0x**
 *  DS18B20_read();        //Чтение байта с шины
 *  DS18B20_temperature(); //Чтение температуры
 *  
 *  Не забудьте установить резистор сопротивлением 4.7k-10k между выводами DQ и Vdd
 *  
 *  Не забудьте отконфигурировать выводы и настройки в строках #define и в функции DS18B20_init
 *  
 *  Перед началом работы нужно записать конфигурационные байты: 
    DS18B20_write(SKIP_ROM);
    DS18B20_write(WRITE_SCRATCHPAD);
    DS18B20_write(0x7F);
    DS18B20_write(0x7F);
    DS18B20_write(0x7F);
*/

#define DQ_in() DDRD &=~ (1<<5) //DQ - вход (высокий уровень на шине благодаря внешней подтяжке)
#define DQ_out() DDRD |= (1<<5) //DQ - выход
#define DQ_pin() PIND & (1<<5)  //Проверка состояния DQ

unsigned char DS18B20_init(void) {
  unsigned char ds18b20_ok = 0x00;
  PORTD &=~ (1<<5);             //Низкий уровень на шине, если выход
  DQ_out();
  _delay_us(500);
  DQ_in();
  _delay_us(80);
  if(!(DQ_pin())) {
    ds18b20_ok = 0xFF;  
  }
  _delay_us(500);
  return ds18b20_ok;
}

#define SKIP_ROM         0xCC
#define CONVERT          0x44
#define WRITE_SCRATCHPAD 0x4E
#define READ_SCRATCHPAD  0xBE


void DS18B20_write(unsigned char ds18b20_data) {
  cli();
  for(int i = 0; i < 8; i++) {
    if(ds18b20_data & (1<<i)) {     
      DQ_out();
      _delay_us(10);
      DQ_in();
      _delay_us(55);
    }
    else {
      DQ_out();
      _delay_us(65);
      DQ_in();
      _delay_us(5);
    }
  }
  sei();
}

unsigned char DS18B20_read(void) {
  cli();
  unsigned char ds18b20_rdata = 0;
  for(int i = 0; i < 8; i++) {
    DQ_out();
    _delay_us(3);
    DQ_in();
    _delay_us(10);    
    if(DQ_pin()) {
      ds18b20_rdata |= (1<<i);
    }
    _delay_us(50);
  }
  sei();
  return ds18b20_rdata;
}
int DS18B20_temperature(void) {
  unsigned char ds18b20_temp;
  int ds18b20temperature = 0;
  if (DS18B20_init() > 0) {
    DS18B20_write(SKIP_ROM);
    DS18B20_write(CONVERT);
    _delay_ms(1000);
    DS18B20_init();
    DS18B20_write(SKIP_ROM);
    DS18B20_write(READ_SCRATCHPAD);
    ds18b20temperature = DS18B20_read();
    ds18b20temperature += DS18B20_read() << 8;
    ds18b20temperature *= 10;
    ds18b20temperature /= 16;
  }
  return ds18b20temperature;
}
