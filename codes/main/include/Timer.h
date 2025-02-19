#include <avr/io.h>
#include <avr/interrupt.h>
uint64_t Times_up =  0;
uint16_t seconds = 0;
uint16_t endSecond = 20;
void Disable_Timer_interrupt(){
    TIMSK &= ~(1 << OCIE1A);
}
void timer1_init() {  
    // Set Timer1 to CTC mode  
    TCCR1A = 0;                  // Normal mode  
    TCCR1B = (1 << WGM12);      // CTC mode with OCR1A  

    // Set the compare value for 10 seconds delay  
    OCR1A = 60000;               // 1 seconds at 8,192 prescaler  

    // Enable Timer1 compare interrupt  
    TIMSK |= (1 << OCIE1A);      // Enable output compare interrupt for OCR1A  
    
    // Set the prescaler to 8192  
    TCCR1B |= (1 << CS12);       // Prescaler of 8192  

    // Enable global interrupts  
    sei();                        // Enable global interrupts  
} 

// Timer0 Compare Match Interrupt Service Routine  
ISR(TIMER1_COMPA_vect) {  
    // This function is called every 10 seconds  
    // You can toggle a pin, increment a counter, etc.  
    // For example, toggle PORTC0  
    PORTD ^= (1 << PD6); // Toggle PC0

    // Count every second!
    seconds++;
    if(seconds >= endSecond) {
        Times_up = 1;
        PORTD |= (1 << PD6); // Toggle PC0
        // seconds = 0;
    }
} 
void Start_15min_Timer() { 
    DDRD |= (1 << PD6); 
    seconds = 0;
    timer1_init();  

}  