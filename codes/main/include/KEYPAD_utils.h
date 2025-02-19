#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 

#define KEY_PRESS_PIN PD2  // INT0 pin (for button press detection)  

// Key matching table
const char keypad_map[4][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};

// Configure INT0 (PD2) to trigger on falling edge  
void interrupt_init() {
    GICR |= (1 << INT0); // Enable external interrupt INT0 
    MCUCR |= (1 << ISC01) | (1 << ISC00); // Rising edge generates an interrupt
    DDRD &= ~(1 << KEY_PRESS_PIN);  // Set PD2 as input (INT0)  
    PORTD &= ~(1 << KEY_PRESS_PIN);   // Enable pull-down on PD2  
    sei(); // Enable global interrupts 
}

void setupKeyPad() {  
    DDRA = 0x0F;  // PA0-PA3 as output (rows), PA4-PA6 as input (columns)  
    PORTA=0xF0;   // Configure the keypad rows and columns  
    
    interrupt_init();
}  

char scan_keypad() {
    for (uint8_t row = 0; row < 4; row++) {
        PORTA = ~(1 << row); // Activate the desired row (LOW)
        _delay_us(10); 

        for (uint8_t col = 0; col < 3; col++) {
            if (!(PINA & (1 << (col + 4)))) { 
                PORTA=0xF0;   // Configure the keypad rows and columns 
                return keypad_map[row][col];
            }
        }
    }
    PORTA=0xF0;   // Configure the keypad rows and columns 
    return '\0'; // If no key is pressed
}
