#pragma once
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

#define F_CPU1 16000000UL
#define BAUD 9600
#define MYUBRR ((F_CPU1 / 16 / BAUD) - 1)

#define EEPROM_START_ADDRESS 0x00  
#define STUDENT_CODE_LENGTH 8 
uint8_t inited_flag = 0;
extern volatile char number_of_students;
extern volatile char student_code[8];

void USART_init(unsigned int ubrr) {
    UBRRL = (unsigned char)ubrr;
    UBRRH = (unsigned char)(ubrr >> 8);
    UCSRB = (1 << RXEN) | (1 << TXEN);
    UCSRC = (1 << UCSZ1) | (1 << UCSZ0); // Set UCSZ1 and UCSZ0 for 8-bit data
}

void USART_Transmit(unsigned char data)
{
    while(!(UCSRA &(1<<UDRE)));
    UDR = data; 
}
void print_StudentsID(){
    uint16_t read_address = EEPROM_START_ADDRESS;
    char code[STUDENT_CODE_LENGTH + 1];  // +1 for '\0'
    char t[17];
    strcpy(t, "Num_of_Students:");
    for(uint8_t i=0;i < 17;i++){
        USART_Transmit(t[i]);
    }
    char str[3];
    sprintf(str, "%d", number_of_students); 
    for(uint8_t i=0; i< 3;i++){
        USART_Transmit(str[i]);
    }   
    USART_Transmit('\r'); 
    USART_Transmit('\n'); 
    for (uint8_t k = 0; k < number_of_students; k++) {  
        for (uint8_t i = 0; i < STUDENT_CODE_LENGTH; i++) {
            code[i] = eeprom_read_byte((const uint8_t*)read_address++); 
        }
        code[STUDENT_CODE_LENGTH] = '\0';  
        for(uint8_t i=0; i< 9;i++)
            USART_Transmit(code[i]);
        USART_Transmit('\r'); 
        USART_Transmit('\n');
     
    }
}
void EEPROM_VirtualPrint_stdID() {
    if(!inited_flag){
        inited_flag = 1;
        USART_init(MYUBRR); // Initialize USART with the correct baud rate
    }
    print_StudentsID();
}