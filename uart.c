#include "uart.h"
#include <avr/interrupt.h>
#ifndef BUILD
#endif

#define F_CPU 16000000
#define BLOCKLEN 16
#define UCSR0B_INIT 0b00011000 //Sets up UART without receive interrupt
#define UCSR0C_INIT 0b00001110
#define UART_BUF_SIZE 64

volatile unsigned char RxDBuff0[UART_BUF_SIZE];
volatile unsigned char  RxDCnt0;
volatile unsigned char  *RxDBuffWR0, *RxDBuffRD0;

// ******************************************************
//	Handle for Rx Interrupt 
//******************************************************

ISR (USART_RX_vect)  {  // Interrupt handler for receive complete interrupt 

	unsigned char key;
	key = UDR0;            // Get UDR --> Buff 
	*RxDBuffWR0 = key;
    RxDCnt0++;
    if (++RxDBuffWR0 >= RxDBuff0 + UART_BUF_SIZE) // Wrap Pointer 
        RxDBuffWR0 = RxDBuff0;
}

uint8_t init_uart(uint32_t baud){
    // Set Baud Rate (UBRR)
    uint16_t baudreg = (uint16_t)(F_CPU / (16*baud)) - 1;
    // uint16_t baudReg = (F_CPU / (16*baud)) - 1; // Asynchronus baud rate register calculation regular speed
    UBRR0H = baudreg>>8;
	UBRR0L = baudreg;
    //Set USCR0(ABC)
    //UCSR0A = (1<<U2X0);
    UCSR0B = UCSR0B_INIT;
    UCSR0C = UCSR0C_INIT;
	RxDBuffWR0  = RxDBuffRD0 = RxDBuff0;
	RxDCnt0 = 0;
    return 0;
}

//Transmit buffer (8 bit register) = UDR0

void uart_handshake(void){
    uint8_t c;
    while(1){
        recv_char(&c);
        if(c=='2'){
            break;
        }
        send_char('2');
    }
}

uint8_t send_char(const uint8_t c){
    while(!(UCSR0A & (1<<UDRE0))); //Wait for communications buffer to be empty
    UDR0 = c; //Place new character into buffer
    return 0;
}

uint8_t send_block(const uint8_t *blk){
    for(int i=0;i<BLOCKLEN;i++){
        send_char(blk[i]);
    }
    return 0;
}

uint8_t send_line(const uint8_t *ln){
    for(int i=0;ln[i]!='\n';i++){
        send_char(ln[i]);
    }
    send_char('\n');
    return 0;
}

uint8_t recv_char(uint8_t *c){
    while(!(UCSR0A & (1<<RXC0)));
    if(!(UCSR0A & ((1<<DOR0) | (1<<FE0)))){
        *c = UDR0;
    }
    else{
        return 1;
    }
    return 0;
}

uint8_t recv_block(uint8_t *blk){
    for(int i=0;i<BLOCKLEN;i++){
        if(recv_char(&blk[i])){
            return 1;
        }
    }
    return 0;
}

uint8_t recv_line(uint8_t *ln){
    uint16_t i = 0;
    do{
        if(recv_char(&ln[i])){
            return 1;
        }
    }while(ln[i++]!='\n');
    return 0;
}

uint8_t compute_crc8(const uint8_t *ln, uint16_t len){
    uint8_t crc = 0;
    const uint8_t *data = ln;
    while(len--){
        crc ^= *data++;
    }
    return crc;
}

// ************************************************
//	Call this function to check for the presence of a character in the
//	Rx Buffer.  The function returns the character count.
// ************************************************

unsigned char  uart0_RxCount(void) {

	return(RxDCnt0);

}