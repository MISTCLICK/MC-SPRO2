#define F_CPU 16000000UL                             // define cpu frequency
#define BAUD 115200                                  // define baud rate
#define BAUD_PRESCALER ((F_CPU / (BAUD * 16UL)) - 1) // define baud prescaler

#include <avr/io.h>

volatile char usart_buffer[10];
volatile int endcount = 0;
volatile int indexs = 0;

void usart_init(void);

void transmit(uint16_t);

void usart_send(unsigned char);

int read_button(void);

void send_command(const char *cmd)
{

  while (*cmd)
  { // Loop through each character in the command string

    usart_send(*cmd++); // Send one character at a time via USART
  }

  usart_send(0xFF); // end with 3 0xFF
  usart_send(0xFF);
  usart_send(0xFF);
}

void usart_send(unsigned char data)
{
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  UDR0 = data; // data to be sent
}

void usart_init(void)
{
  UBRR0H = (uint8_t)(BAUD_PRESCALER >> 8); // save baud prescaler in 2 registers
  UBRR0L = (uint8_t)(BAUD_PRESCALER);

  UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (0 << RXCIE0); // enable tx and rx
  UCSR0C = ((1 << UCSZ00) | (1 << UCSZ01));             // set character size as 8 bit, 1 stop bit

  // UCSR0B |= (1<<RXCIE0); //enable interrupt when recieving data
}

void transmit(uint16_t rpm)
{

  int rpmh = rpm >> 8; // turn rpm into 2 uint8_t
  int rpml = rpm;
  printf("%d %d \n", rpmh, rpml);

  // char buffer[16]; // Temporary chracter array, chosen to be large enough to hold the entire formatted string

  // sprintf(buffer, "t2.txt=\"%u\"", rpm); // send  rpm information to nextion
  // send_command(buffer);                  // send the command string to the netion display

  // sprintf(buffer, "t5.txt=\"%u\"",angle); //send angle information to the nextion
  // send_command(buffer);

  // sprintf(buffer, "t4.txt=\"%u\"", t); //send timeelapsed information to the nextion
  // send_command(buffer);
}

/*
int read_button(void)
{

  char input_buffer[7]; //we expect a 7 byte message
  for (int i = 0; i < 7; i++)
  {
    while (!(UCSR0A & (1 << RXC0))); //recieve 1 byte
    input_buffer[i] = UDR0; //save byte in buffer
    PORTB|= (1<<PB5);

  }

  if (input_buffer[1] == 1) //if second byte recieved is 1 then the button has been pressed
  {
    PORTB|= (1<<PB5);
    return 1;
  }
  PORTB|= (1<<PB5);
  return 0;
}

*/
