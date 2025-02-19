#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h> 
#include <stdio.h>

//#include "LCD_utils.h"

#define EEPROM_START_ADDRESS 0x00  
#define STUDENT_CODE_LENGTH 8     

extern volatile char number_of_students;


volatile uint16_t eeprom_write_address = EEPROM_START_ADDRESS;

void save_student_code_to_eeprom(volatile char input_code[STUDENT_CODE_LENGTH]) {
    for (uint8_t i = 0; i < STUDENT_CODE_LENGTH; i++) {
        eeprom_write_byte((uint8_t*)eeprom_write_address++, input_code[i]); 
    }
}

uint8_t std_code_existance(volatile char input_code[STUDENT_CODE_LENGTH]){
    uint16_t read_address = EEPROM_START_ADDRESS;
    char stored_code[STUDENT_CODE_LENGTH]; 

    for (uint8_t k = 0; k < number_of_students; k++) {  
        for (uint8_t i = 0; i < STUDENT_CODE_LENGTH; i++) {
            stored_code[i] = eeprom_read_byte((const uint8_t*)read_address++); 
        }

        uint8_t is_match = 1;
        for (uint8_t i = 0; i < STUDENT_CODE_LENGTH; i++) {
            if (stored_code[i] != input_code[i]) {
                is_match = 0;
                break;
            }
        }

        if (is_match) {
            return 1; 
            return read_address - STUDENT_CODE_LENGTH; // Return the start address of the found code
        }
    }
    return 0;
}


uint8_t delete_student(volatile char input_code[STUDENT_CODE_LENGTH]){
    if(!std_code_existance(input_code))
    {
        return 0;
    }
    volatile char compare_code[STUDENT_CODE_LENGTH];
    uint16_t read_address = EEPROM_START_ADDRESS;
    eeprom_write_address = EEPROM_START_ADDRESS;

    for (uint8_t k = 0; k < number_of_students; k++) {  
        eeprom_read_block((void*)compare_code, (const void*)read_address, STUDENT_CODE_LENGTH);
        read_address += STUDENT_CODE_LENGTH;

        uint8_t is_match = 1;
        for (uint8_t i = 0; i < STUDENT_CODE_LENGTH; i++) {
            if (compare_code[i] != input_code[i]) {
                is_match = 0;
                break;
            }
        }


        if (!is_match) {
            save_student_code_to_eeprom(compare_code);
        }
    }
    return 1;
    
}




void read_student_codes_from_eeprom() {
    uint16_t read_address = EEPROM_START_ADDRESS;
    char code[STUDENT_CODE_LENGTH + 1];  // +1 for '\0'

    // lcdcmd(0x01); // clear LCD
    // lcdcmd(0x80); // first line
    GLCD_Clear();
    GLCD_Render();
    GLCD_GotoLine(0);
    GLCD_GotoX(1);
    GLCD_PrintString("Num_of_Students:");
    GLCD_Render();
    //lcd_Print("Num_of_Students:");
    //lcdcmd(0xC0); // second line
    GLCD_GotoLine(2);
    GLCD_GotoX(1);
    char str[3];
    sprintf(str, "%d", number_of_students);  
    GLCD_PrintString(str);
    GLCD_Render();
    //lcd_Print(str);
    _delay_ms(1000); 
    GLCD_Clear();
    GLCD_Render();
    //lcdcmd(0x01); // clear LCD
    //lcdcmd(0x80); // first line
    GLCD_GotoLine(0);
    GLCD_GotoX(1);
    GLCD_PrintString("Student Code:");
    GLCD_Render();
    //lcd_Print("Student Code:");

    for (uint8_t k = 0; k < number_of_students; k++) {  
        for (uint8_t i = 0; i < STUDENT_CODE_LENGTH; i++) {
            code[i] = eeprom_read_byte((const uint8_t*)read_address++); 
        }
        code[STUDENT_CODE_LENGTH] = '\0';  
        GLCD_GotoLine(2);
        GLCD_GotoX(1);
        GLCD_PrintString(code);
        GLCD_Render();
        _delay_ms(2000); 
        // lcdcmd(0xC0); // second line
        //lcd_Print(code);
        GLCD_ClearLine(2);
        GLCD_GotoX(1);
        GLCD_Render();
        

    }
    GLCD_Clear();
    GLCD_Render();
}
