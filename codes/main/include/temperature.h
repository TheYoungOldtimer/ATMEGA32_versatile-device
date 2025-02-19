#include <avr/io.h>
#include <util/delay.h> 

//#define F_CPU 8000000UL
#define v_out PA7

void ADC_Init() {
    DDRA &= ~(1 << v_out);  // Set pin PA7 as input
    ADCSRA = 0x87;          // Enable ADC and select clock divided by 128
    ADMUX = (1 << REFS1) | (1 << REFS0);  // Internal reference voltage 1.1V
}

int16_t ADC_Read(uint8_t channel) {
    int16_t adc_value;

    ADMUX = (ADMUX & 0xF8) | (7 & 0x07);

    ADCSRA |= (1 << ADSC);            // start conversion
    while ((ADCSRA & (1 << ADIF)) == 0); // wain for conversion
    ADCSRA |= (1 << ADIF);            // Clear ADIF flag

    // Read the raw ADC value
    adc_value = ADCL;
    adc_value |= (ADCH << 8);

    adc_value = (adc_value>>2);
    return adc_value;
}

void ADC_end() {
    ADCSRA &= ~(1 << ADEN);  // Disable ADC
}
