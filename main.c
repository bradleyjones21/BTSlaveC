/*
 * GccApplication1.c
 *
 * Created: 2022-05-23 10:50:13 PM
 * Author : bradt
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "uart.h"


#define INTER_START "INTR"
#define REAC_START  "REAC"
#define MISS_MODE   "MISS"
#define TIMEOUT_SLOW 10
#define TIMEOUT_MED 5
#define TIMEOUT_FAST 2.5

#define TESTMODE 1 //1: running without bluetooth to receive, 0: running normally

const int buttonPin = 9;
int buttonState;
int temp = -1;
uint8_t miss_mode = 0;
int ovfl = 0;
uint8_t timer = 0;
uint8_t had_vibration = 0;
char tempStr[40] = "\0";

ISR(TIMER1_OVF_vect){
	ovfl++;
}

ISR(PCINT0_vect){
	buttonState = 1;
	_delay_ms(10);
}

int main(void)
{
	 uint8_t timeout;
     buttonState = 0;
     init_uart((uint32_t)115200);
     _delay_ms(100);
	 send_line((unsigned char*) "AT+DEFAULT\r\n");
     _delay_ms(100);
	 send_line((unsigned char*) "AT+BAUD4\r\n");
     _delay_ms(100);
     init_uart((uint32_t)9600);
     _delay_ms(100);
	 send_line((unsigned char*)"AT+DEFAULT\r\n");
     _delay_ms(100);
	 send_line((unsigned char*)"AT+STARTEN1\r\n");
     _delay_ms(100);
     TCCR1A = 0;
     TCCR1B = 0;
     TCCR1B |= 0b00000001;
     TIMSK1 &= 0b00000000;
     TCNT1 = 0;
     //pcint1
     PCICR |= 0b00000111;    // turn on all ports
     PCMSK0 |= 0b00000010;
     sei();
	 uint8_t *c = malloc(40);
	 uint8_t *reac = (uint8_t*)"REAC";
    while (1) 
    {
		if(buttonState==1){
			//Serial.print("Button pressed\n");
		}
		if (uart0_RxCount()||TESTMODE){//Serial.available()) {
			if(TESTMODE){
				strcpy((char *) c,"REAC\n");
			}else{
				//recv_line(c);
			}
				
			send_line(c);
			_delay_ms(10);
			if (strcmp((const char*)c,(const char*)reac)){
				strcpy((char *) c,"");
				miss_mode = 0;
				//buzzer on
				//led on
				//start/reset timer
				send_line((unsigned char*)"Reac Confirmed\r\n");
				TIMSK1 |= 0b00000001;
				TCNT1   = 0;
				ovfl    = 0;
				timeout = 1;
				while(timeout){
					
					//Serial.write("test\n");
					timer = (TCNT1 + ovfl*65536)/16000; //time in ms
					if (timer > TIMEOUT_SLOW*1000){
						TIMSK1 &= 0b00000000;
						_delay_ms(10);
						send_line((unsigned char*)"timeout\r\n");
						timeout = 0;
					}
					//if(buttonState=1){
					//timer+="\n";
					//Serial.print("button pressed");
					//TIMSK1 &= 0b00000000;
					//break;
					//}
				}
			}
			//temp = c.indexOf(INTER_START);
			if (temp == 0){
				strcpy((char *)c,"");
				miss_mode = 0;
				//turn on led and buzzer
				//delay by the amount received
				//end
			}
			//temp = c.indexOf(MISS_MODE);
			strcpy((char *)c,"");
			if (temp == 0){
				miss_mode = 1;
			}
		}
    }
}

