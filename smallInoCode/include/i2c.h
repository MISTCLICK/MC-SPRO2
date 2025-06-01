#include <avr/io.h>
#include <util/twi.h>

#define MAGNETIC_ENCODER 0b0110110 // adress of magnetic encoder
#define SERVER 0x69                // nice
#define SCL_CLOCK 100000UL         // 100 kHz standard I2C
                                   /*
                                   void TWI_start(void);
                                   void TWI_init(void);
                                   void TWI_stop(void);
                                   void TWI_write(uint8_t);
                                   uint8_t TWI_read_ack(void);
                                   uint8_t TWI_read_nack(void);
                                   
                                   void TWI_init(void){
                                       TWSR = 0; //prescaler 0
                                       TWBR = ((F_CPU/SCL_CLOCK) - 16) / 2; // Bit rate
                                       TWCR = (1 << TWEN); // Enable TWI
                                   }
                                   
                                   void TWI_start(void){
                                       TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); //send start condition
                                       while (!(TWCR & (1<<TWINT))); //wait til it has been sent, TWINT is set as 1 when it has been sent
                                   }
                                   
                                   
                                   void TWI_stop(void){
                                       TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO); //send stop condition
                                       while (TWCR & (1<<TWSTO)); // Wait until stop condition has been sent, TWSTO is cleared automatically when done
                                   }
                                   
                                   
                                   void TWI_write(uint8_t message){
                                       TWDR = message; //load message into TWDR
                                       TWCR = (1<<TWINT) | (1<<TWEN); //start transmission of data
                                       while (!(TWCR & (1<<TWINT))); //wait for transmission to be complete
                                   }
                                   
                                   uint8_t TWI_read_ack(void){ //the magnetic encode will continue to send data untill an nack has been recieved
                                       TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // send an ack
                                       while (!(TWCR & (1 << TWINT))); //wait for data to be recieved
                                       return TWDR; //return data recieved
                                   }
                                   
                                   uint8_t TWI_read_nack(void) { //send this to recieve data and make it stop sending more
                                       TWCR = (1 << TWINT) | (1 << TWEN); // here we dont send an ack, so therefore its a nack, since it has not been acknowledged
                                       while (!(TWCR & (1 << TWINT))); //wait for data to be recieved
                                       return TWDR; //return data recieved
                                   }*/