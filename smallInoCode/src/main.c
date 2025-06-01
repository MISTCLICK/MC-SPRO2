/*
 * main.c
 *
 * Created: 11/9/2023 10:43:27 AM
 * Author : Niko
 */

#define UNIT_OF_TIMER

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
// #include "ADCLibrary.h"
#include <avr/interrupt.h>
#include "init.h"
// #include "i2c.h"
#include "usart.h"
#include "nikos_usart.h" //usart.h is made by alin, nikos_usart.h is made by me
#include "functions.h"

#include "i2cmaster.h"

int state = 1;         // state machine
unsigned int dt = 0;   // save time since last speedmeasurement in this variable to calculate
volatile int time = 0; // use to measure time since last speed measurement

uint16_t angle = 0;
uint16_t new_angle = 0;
float difference = 0;
uint16_t voltage = 0;
uint16_t rpm = 0;
uint16_t timeelapsed = 0; // count one every 5 ms
float degree;
float degreeafter;
int stop = 0;
int stopcount = 0;

int main(void)
{
  // init(); //call all inits
  // TWI_init();
  i2c_init();
  uart_init();
  io_redirect();
  DDRB |= (1 << PB5);
  TCCR2A |= (0 << WGM20) | (1 << WGM21);                            // use these to set mode (already in ctc mode)
  TCCR2B |= (0 << WGM22) | (1 << CS20) | (1 << CS21) | (0 << CS22); // use ctc mode and prescaler
  OCR2A = 249;                                                      // set timer count up to here
  TIMSK2 |= (1 << OCIE2A);                                          // interrupt when OCR0A is reached

  TCNT0 = 0;

  sei();

  while (1)
  {

    if (state == 0)
    {
      _delay_ms(100);
      if (read_button())
      { // yes this is a major bug since its a blocking call that can keep the program stuck forever, but the 2 people supposed to make this work didnt do anything so this is my patchwork solution
        state = 1;
      }
    }
    // state 0 means wait for input
    if (state == 1)
    { // state 1 is first measurement

      // timer0_init(); //start timer, (it isnt needed before this moment anyways)
      motor_init();
      motor_control(255, 1);
      angle = 0; // read the start angle and save it
      state = 2;
      time = 0;
    }

    if (state == 2)
    { // when program starts activate this

      if (time > 4)
      { // after 5 ms, we measure our speed once every 5 ms

        new_angle = AS5600_ReadAngle(); // new angle is read angle
        degree = (new_angle * 360.0) / 4096.0;
        // new_angle = degree;

        timeelapsed++; // multiply by 5 to get time in ms

        if (degree < degreeafter)
        {
          difference = degree + 360 - degreeafter;
        }
        else
        {
          difference = degree - degreeafter;
        }
        degreeafter = degree;

        rpm = calculate_RPM(difference); // call this to calculate speed
        // voltage=ADC_read(); //measure voltage, might not be used
        // transmit information
        // rpm = timeelapsed;
        printf("%d \n", rpm);
        // transmit(rpm);

        time -= 5; // wait til 5 ms has passed again, we subtract 5 instead of time=0 since time is still passing while this is running, making sure we get 5 ms between each measurement

        if (rpm < 4)
        { // if speed is below 1 start a counter
          stopcount++;
        }
        else
        { // reset count if it goes above 1
          stopcount = 0;
        }
        if (stopcount == 200)
        { // if speed is below 1 for 1 second it will go to state 3
          state = 3;
        }
        transmit_information(rpm, degree, timeelapsed);
      }
    }
  }
  return 0;
}

/* we now use i2c instead
ISR(ANALOG_COMP_vect){ //when the voltage on PD6 (AIN0) is higher than the voltage on PD7 (AIN1) this activates (rising edge),
  dt=time; //saves the time difference since last time counted
  time=0; //restart time
}

ISR(ADC_vect){
  adc_measurement = ADCL; //get read ADC signal
  ADCSRA |= (1 << ADIE); //Start new ADC conversion
}
*/

/*
ISR(__NONE__YET___){ //one rotation
  dt=time; //saves the time difference since last time counted
  time=0; //restart time
}*/

ISR(TIMER2_COMPA_vect)
{
  time++; // keeps track of time since last measurement
}

/* not used rn since we have a worse but working solution
ISR(USART_RX_vect){ //usart recieve function
  if(read_button()){
    PORTB|= (1 << PB5);
  }

}
  */
/*

int next_read_button_press(void);
int next_read_button_press(void)
{
    unsigned char readBuffer[4];
    readBuffer[0] = 0x97;
    readBuffer[1] = 0x96;
    readBuffer[2] = 0x95;
    readBuffer[3] = 0x94;

    // while (UCSR0A & (1 << RXC0)) {

    // if(readBuffer[0]==0x68){
    for (int i = 0; i < 4; i++)
    { // reads the value of the response
        readBuffer[i]=UDR0;
               // printf("%x ",readBuffer[i]);
    }
    //  printf("%x \n", readBuffer[1]);
    if (readBuffer[0] == 0x01 || readBuffer[1] == 0x01)
    {
        return 1;
    }
    else
    {
        return 0;
    }
    // }
    // }
}*/

int read_button(void)
{
  char input_buffer[9];
  for (int i = 0; i < 7; i++)
  {
    while (!(UCSR0A & (1 << RXC0)))
      ;
    input_buffer[i] = UDR0;

    if (input_buffer[i] == 0x65)
    {

      state = 1;
      return 1;
      break;
    }
  }

  if (input_buffer[0] == 0x65 || input_buffer[2] == 0x01)
  {

    state = 1;
    return 1;
  }
  return 0;
}