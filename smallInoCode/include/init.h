
//#include "ADCLibrary.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void init(void);

void init(void){
    //timer0_init(); 
    ADC_init();
    sei();
}




void timer0_init(void);

void timer0_init(void){
  TCCR0A |= (0<<WGM00) | (1<<WGM01); //use these to set mode (already in ctc mode)
  TCCR0B |= (0<<WGM02) | (0<<CS00) | (1<<CS01) | (1<<CS02); //use ctc mode and prescaler
  OCR0A = 249; //set timer count up to here
  TIMSK0 |= (1<<OCIE0A); //interrupt when OCR0A is reached

  TCNT0 = 0;
}