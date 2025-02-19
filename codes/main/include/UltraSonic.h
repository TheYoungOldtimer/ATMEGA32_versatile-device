

#include <avr/io.h>   // Header for atmega32
#include <util/delay.h>      // To use for delay function
#include <stdint.h>
#include <stdio.h>
// #include <avr/interrupt.h>

// Port definitions for easier reference
#define PC0 0  // Pin 0 of PORTC
#define PC1 1  // Pin 1 of PORTC
#define PB0 0  // Pin 0 of PORTB (Trigger)
#define PB1 1  // Pin 1 of PORTB (Echo)
#define PD0 0
#define PD3 3
#define PD2 2


// LCD Definitions
#define lcd_cmd_port DDRD
#define lcd_portd PORTD

#define lcd_portc PORTC   // LCD connected to PORTD
#define lcd_data_dir DDRC // LCD data direction register
#define rs PD0            // RS pin connected to PD0
#define en PD3            // Enable pin connected to PD1

// Ultrasonic Sensor Definitions
#define US_PORT PORTB     // Ultrasonic sensor connected to PORTB
#define US_PIN PINB       // Ultrasonic PIN register
#define US_DDR DDRB       // Ultrasonic data direction register

#define US_TRIG_POS PB0   // Trigger pin connected to PB0
#define US_ECHO_POS PB1   // Echo pin connected to PB1

// Error indicators
#define US_ERROR -1       // Error indicator
#define US_NO_OBSTACLE -2 // No obstacle indicator
uint8_t Exit_flag = 0;
uint8_t Exit_en = 0;
volatile int count = 0; // Counting variable

void HCSR04Init();
void HCSR04Trigger();
void lcd_command(unsigned char);
void lcd_clear();
void lcd_print(char *);
void lcd_setCursor(unsigned char, unsigned char);
uint16_t GetPulseWidth();

// volatile char pressed_key = '\0';

// ISR(INT0_vect) {
//     _delay_ms(10); // (Debounce)
//     pressed_key = scan_keypad();
     
// }


void HCSR04Init() {
    US_DDR |= (1 << US_TRIG_POS); // Trigger pin as output
    US_DDR &= ~(1 << US_ECHO_POS); // Echo pin as input
}

void HCSR04Trigger() {
    US_PORT |= (1 << US_TRIG_POS); // Set trigger pin high
    _delay_us(15);                  // Wait for 15 microseconds
    US_PORT &= ~(1 << US_TRIG_POS); // Set trigger pin low
}

uint16_t GetPulseWidth() {
    uint32_t i, result;

    // Wait for rising edge on Echo pin
    for (i = 0; i < 600000; i++) {
        if (!(US_PIN & (1 << US_ECHO_POS)))
            continue;
        else
            break;
    }

    if (i == 600000)
        return US_ERROR; // Timeout error if no rising edge detected

    // Start timer with prescaler 8
    TCCR1A = 0x00;
    TCCR1B = (1 << CS11);
    TCNT1 = 0x00; // Reset timer

    // Wait for falling edge on Echo pin
    for (i = 0; i < 600000; i++) {
        if (!(US_PIN & (1 << US_ECHO_POS)))
            break;  // Falling edge detected
        if (TCNT1 > 60000)
            return US_NO_OBSTACLE; // No obstacle in range
    }

    result = TCNT1; // Capture timer value
    TCCR1B = 0x00; // Stop timer

    if (result > 60000)
        return US_NO_OBSTACLE;
    else
        return (result >> 1); // Return the measured pulse width
}

// void initialize() {
//     lcd_cmd_port = 0x0F;
//     lcd_data_dir = 0xF0;   // Set LCD data direction
//     _delay_ms(15);
//     lcd_command(0x02); // Return home
//     lcd_command(0x28); // 4-bit mode, 2 lines, 5x7 font
//     lcd_command(0x0C); // Display on, cursor off
//     lcd_command(0x06); // Increment cursor
//     lcd_command(0x01); // Clear display
//     _delay_ms(2);
// }

// void lcd_command(unsigned char cmnd) {
//     // Send upper nibble
//     lcd_portd = lcd_portd & 0x0F;
//     lcd_portc = cmnd & 0xF0;

//     lcd_portd &= ~(1 << rs);  // RS pin low for command
//     lcd_portd |= (1 << en);   // Enable high
//     _delay_us(1);
//     lcd_portd &= ~(1 << en);  // Enable low

//     // Send lower nibble
//     lcd_portd = lcd_portd & 0x0F;
//     lcd_portc = (cmnd << 4);

//     lcd_portd |= (1 << en);   // Enable high
//     _delay_us(1);
//     lcd_portd &= ~(1 << en);  // Enable low
//     _delay_ms(2);
// }

// void lcd_clear() {
//     lcd_command(0x01); // Clear LCD
//     _delay_ms(2);
//     lcd_command(0x80); // Return to first line
// }

// void lcd_print(char *str) {
//     int i;
    
//     for (i = 0; str[i] != 0; i++) {
//         lcd_portd = lcd_portd & 0x0F ;
//         lcd_portc = str[i] & 0xF0;
//         lcd_portd |= (1 << rs);  // RS pin high for data
//         lcd_portd |= (1 << en);  // Enable high
//         _delay_us(1);
//         lcd_portd &= ~(1 << en); // Enable low

//         lcd_portd = lcd_portd & 0x0F;
//         lcd_portc = str[i] << 4;
//         lcd_portd |= (1 << en);  // Enable high
//         _delay_us(1);
//         lcd_portd &= ~(1 << en); // Enable low
//         _delay_ms(2);
//     }
// }

// void lcd_setCursor(unsigned char x, unsigned char y) {
//     unsigned char adr[] = {0x80, 0xC0};
//     lcd_command(adr[y - 1] + x - 1);
//     _delay_us(100);
// }

void Ultrasoinc() {
    Exit_en = 1;
    char numberString[16];  // Buffer to hold distance string
    uint16_t pulseWidth;    // Pulse width from echo
    int distance;           // Calculated distance


    //initialize();           // Initialize LCD
    HCSR04Init();           // Initialize ultrasonic sensor

    while (!Exit_flag) {
        _delay_ms(1000);  // Delay for sensor stability

        HCSR04Trigger();              // Send trigger pulse
        pulseWidth = GetPulseWidth();  // Measure echo pulse

        if (pulseWidth == US_ERROR) {
            // lcd_clear();
            // lcd_setCursor(1, 1);
            // lcd_print("Error");        // Display error message
            GLCD_Clear();
            GLCD_Render();
            GLCD_GotoLine(0);
            GLCD_GotoX(1);
            GLCD_PrintString("Error");
            GLCD_Render();
            _delay_ms(1000);
            GLCD_Clear();
            GLCD_Render();
            
        } else if (pulseWidth == US_NO_OBSTACLE) {
            GLCD_Clear();
            GLCD_Render();
            GLCD_GotoLine(0);
            GLCD_GotoX(1);
            GLCD_PrintString("No Obstacle");
            GLCD_Render();
            _delay_ms(1000);
            GLCD_Clear();
            GLCD_Render();
            //lcd_clear();
            //lcd_setCursor(1, 1);
            //lcd_print("No Obstacle");  // Display no obstacle message
        } else {
            distance = (int)((pulseWidth * 0.034 / 2) + 0.5);

            // Display distance on LCD
            sprintf(numberString, "%d", distance); // Convert distance to string
            // lcd_clear();
            // lcd_setCursor(1, 1);
            // lcd_print("Distance: ");
            // lcd_print(numberString); 
            // lcd_print(" cm");

            GLCD_Clear();
            GLCD_Render();
            GLCD_GotoLine(0);
            GLCD_GotoX(1);
            GLCD_PrintString("Distance: ");
            GLCD_PrintString(numberString);
            GLCD_PrintString(" cm");
            GLCD_GotoLine(5);
            GLCD_GotoX(1);
            GLCD_PrintString("Press any key to Quit!");
            GLCD_Render();
            // Counting logic based on distance
            if (distance < 6) {
                count++;  // Increment count if distance is below threshold
            }

            // Update count on LCD only if it changes
            // lcd_setCursor(1, 2); // Move to second line
            GLCD_GotoLine(2);
            GLCD_GotoX(1);
            sprintf(numberString, "%d", count);
            GLCD_PrintString("Count: ");
            //lcd_print("Count: ");
            GLCD_PrintString(numberString);
            GLCD_Render();
            //lcd_print(numberString);
        }
        
    }
    Exit_flag = 0;
    Exit_en = 0;
    GLCD_Clear();
    GLCD_Render();
}



