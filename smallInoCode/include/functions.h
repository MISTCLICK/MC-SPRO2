#include <avr/io.h>
#include "i2c.h"
#include "i2cmaster.h"

uint16_t read_angle(void);
void transmit_information(uint16_t, uint16_t, uint16_t);
uint16_t calculate_RPM(float);
uint16_t float_to_uint16(float);
void motor_init(void);
void motor_control(uint8_t speed, uint8_t direction);

#define AS5600_ADDR 0x36 // 7-bit I2C address of AS5600
uint16_t AS5600_ReadAngle(void);
uint16_t AS5600_ReadAngle(void)
{
  uint16_t angle;

  i2c_start((AS5600_ADDR << 1) | I2C_WRITE); // Start + Write mode
  i2c_write(0x0C);                           // Address of angle high byte register
  i2c_stop();                                // Stop to finish write

  i2c_start((AS5600_ADDR << 1) | I2C_READ); // Start + Read mode
  uint8_t high = i2c_readAck();             // Read high byte
  uint8_t low = i2c_readNak();              // Read low byte
  i2c_stop();                               // Stop communication

  angle = ((uint16_t)high << 8) | low;
  return angle & 0x0FFF; // AS5600 only uses the lower 12 bits
}

uint16_t read_angle(void)
{
  /*
  uint8_t msb; // make 2 variables to store the data, this is most significant byte
  uint8_t lsb; // this is least significant byte
  // the angle will be 2 bytes since the max value an angle can be is 360 degrees and a byte can only hold up to 255

  // uncomment this if interrupts break i2c cli();//temporary disable interrupts to make sure the transmition will be safe, if we read every x ms, we can be 100% sure no interrupts will happen at this moment anyways
  TWI_start();                      // start transmission
  TWI_write(MAGNETIC_ENCODER << 0); // shift it 0 to create XXXX XXX1 where X is the adress, 0 tells it to be put in write mode
  TWI_write(0x0E);                  // tells it to set a pointer to the 0x0E register.  0x0E is the bits from 8:14 and 0x0F is the bits from 0:7. after reading it will increment the pointer
  // TWI_stop(); //this is end of the read request
  _delay_us(10);

  TWI_start();                      // start again and read 2 bytes
  TWI_write(MAGNETIC_ENCODER << 1); // shift it 1 to create XXXX XXX1 where X is the adress, 1 tells it to be put in read mode
  msb = TWI_read_ack();             // save the most significant byte, send an ack to make it continue sending
  lsb = TWI_read_nack();            // save the least significant byte, dont send an ack to make it stop sending data
  TWI_stop();
  // uncomment this if interrupts break i2c sei();
  uint16_t result = (msb << 8) + lsb; // put the 2 bytes together
  return result;
  */
  return 0;
}

/*
void transmit_information(uint16_t rpm, uint16_t angle, uint16_t timepassed)
{
  uint8_t rpmh = rpm >> 8; // turn rpm into 2 uint8_t
  uint8_t rpml = rpm;
  uint8_t angleh = angle >> 8;
  uint8_t anglel = angle;
  uint8_t timeh = timepassed >> 8;
  uint8_t timel = timepassed;

  TWI_start();            // start i2c transmission
  TWI_write(SERVER << 0); // select the other microcontroller, use 0 for write
  TWI_write(rpmh);        // write to it the value of the rpm and voltage
  TWI_write(rpml);
  TWI_write(angleh);
  TWI_write(anglel);
  TWI_write(timeh);
  TWI_write(timel);
  TWI_stop(); // transmission over
}
*/
uint16_t calculate_RPM(float angle)
{
  float rotations = angle / 360;             // how many rotations it has performed
  float rpmm = rotations * 12000;            // we take a measurement every 5 ms, this means 200 measurements per second, this means 200*60=12000 measurements per minute
  uint16_t uint_rpm = float_to_uint16(rpmm); // convert rpm to uint16_t
  return uint_rpm;
}

uint16_t float_to_uint16(float value)
{
  uint16_t converted = (uint16_t)(value * 1000.f + 0.5f); // multiply by 1000 to get 3 significant digets saved, then plus 0,5 to round to integer
  converted /= 1000;
  return converted;
}

void motor_init(void)
{ // initiate the motor with pwm

  DDRD |= (1 << PD6 | 1 << PD5); // set pd6 and pd7 as outputs for direction of the motor

  TCCR0A = (1 << WGM00) | (1 << COM0A1) | (1 << COM0B1); // fast pwm enabled at 8 bits
  TCCR0B = (1 << CS01);                                  // 8 prescaler

  OCR0A = 0; // engine off by default
  OCR0B = 0;
}

void motor_control(uint8_t speed, uint8_t direction)
{ // control the speed and direction

  if (speed > 255)
  {
    speed = 255;
  }

  if (direction == 1)
  { // motor forwards(1)

    OCR0A = speed; // Set duty cycle for PD6 (OC0A)
    OCR0B = 0;     // turn off duty cycle for PD5
  }
  else if (direction == 2)
  {                // motor backwards(2)
    OCR0A = 0;     // turn off duty cycle for PD6 (OC0A)
    OCR0B = speed; // set duty cycle for PD5
  }
  else
  {
    OCR0A = 0;
    OCR0B = 0; // stop
  }
}

void transmit_information(uint16_t rpm, uint16_t angle, uint16_t timepassed)
{
  uint8_t rpmh = rpm >> 8; // turn rpm into 2 uint8_t
  uint8_t rpml = rpm;
  uint8_t angleh = angle >> 8;
  uint8_t anglel = angle;
  uint8_t timeh = timepassed >> 8;
  uint8_t timel = timepassed;

  i2c_start((SERVER << 1) | I2C_WRITE); // start i2c transmission select the other microcontroller, use 0 for write
  i2c_write(rpmh);                      // write to it the value of the rpm and voltage
  i2c_write(rpml);
  i2c_write(angleh);
  i2c_write(anglel);
  i2c_write(timeh);
  i2c_write(timel);
  i2c_stop(); // transmission over
}