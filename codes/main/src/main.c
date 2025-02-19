#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <Font5x8.h>
#include "KS0108.h"

#include "LCD_utils.h"
#include "KEYPAD_utils.h"
#include "eeprom_utils.h"
#include "virtual.h"
#include "UltraSonic.h"
#include "temperature.h"
#include "Timer.h"

#define BUZZER_PIN PD7  


volatile char pressed_key = '\0';
volatile char student_code[8];
volatile char number_of_students = 0;






ISR(INT0_vect) {
    _delay_ms(10); // (Debounce)
    pressed_key = scan_keypad();
    if(Exit_en) 
      Exit_flag = 1;
}



  // GLCD_Clear();
  // GLCD_Render();

  // GLCD_GotoLine(0);
  // GLCD_GotoX(1);
	// GLCD_PrintString("1_submit SC");
  // GLCD_GotoLine(2);
  // GLCD_GotoX(1);
  // GLCD_PrintString("2_Exit");
  // GLCD_Render();


void del_std_code(){
  pressed_key = '\0';
  // lcdcmd(0x01); //clear LCD
  // lcdcmd(0x80);//set the curser to first line
	// lcd_Print("student ID:");
  // lcdcmd(0xC0); //set the curser to 2nd line
    GLCD_Clear();
    GLCD_Render();

    GLCD_GotoLine(0);
    GLCD_GotoX(1);
    GLCD_PrintString("student ID:");
    GLCD_GotoLine(2);
    GLCD_GotoX(1);
    GLCD_Render();
  uint8_t i = 0;

  while(1 ){
    if((pressed_key != '\0') & (pressed_key != '#') & (pressed_key != '*')){
      //lcd_data(pressed_key);
      GLCD_GotoLine(2);
      GLCD_GotoX((i*6)+1);
      GLCD_PrintChar(pressed_key);
      GLCD_Render();
      GLCD_GotoLine(2);
      GLCD_GotoX(1);
      GLCD_Render();
      if(i<8){
        student_code[i] = pressed_key;
      }
      i++;
      pressed_key = '\0';
    }
    else if(pressed_key == '#'){
      if((i == STUDENT_CODE_LENGTH) & (student_code[0] == '4')){
                if(delete_student(student_code)){
                    // lcdcmd(0x01); 
                    GLCD_Clear();
                    GLCD_Render();
                    GLCD_GotoLine(0);
                    GLCD_GotoX(1);
                    //lcdcmd(0x80);
                    number_of_students--;
                    GLCD_PrintString("Student Deleted!");
                    GLCD_Render();
                    //lcd_Print("Student Deleted!");
                    _delay_ms(1500);
                    GLCD_Clear();
                    
                }  
                else{
                    // lcdcmd(0x01); 
                    // lcdcmd(0x80);
                    GLCD_Clear();
                    GLCD_Render();
                    GLCD_GotoLine(0);
                    GLCD_GotoX(1);
                    GLCD_PrintString("Student doesn't exist!");
                    GLCD_Render();
                    //lcd_Print("Student Not Deleted!");
                    _delay_ms(1500);
                    GLCD_Clear();
                    GLCD_Render();
                }

                
            } 
      else {
                // lcdcmd(0x01); 
                // lcdcmd(0x80); 
                GLCD_Clear();
                GLCD_Render();
                GLCD_GotoLine(0);
                GLCD_GotoX(1);
                GLCD_PrintString("Invalid Code!");
                //lcd_Print("Invalid Code!");
                PORTD &= ~(1 << BUZZER_PIN);  //  Buzzer on
                _delay_ms(1000);             
                PORTD |= (1 << BUZZER_PIN); //  Buzzer off
                GLCD_Clear();
                GLCD_Render();
      }
      i = 0;
      pressed_key = '\0';
      break;
    }
    else if(pressed_key == '*'){
      i = 0;
      // lcdcmd(0x10);
      GLCD_ClearLine(2);
      GLCD_GotoX(1);
      GLCD_Render();
      pressed_key = '\0';
    }
  }
  GLCD_Clear();
  GLCD_Render();
}







void get_std_code(){
  pressed_key = '\0';
  //lcdcmd(0x01); //clear LCD
  GLCD_Clear();
  GLCD_Render();
  //lcdcmd(0x80);//set the curser to first line
  GLCD_GotoLine(0);
  GLCD_GotoX(1);
	// lcd_Print("student ID:");
  GLCD_PrintString("student ID:");
  //lcdcmd(0xC0); //set the curser to 2nd line
  GLCD_GotoLine(2);
  GLCD_GotoX(1);
  GLCD_Render();
  uint8_t i = 0;

  while(!Times_up){
    if((pressed_key != '\0') & (pressed_key != '#')& (pressed_key != '*')){
      //lcd_data(pressed_key);
      GLCD_GotoLine(2);
      GLCD_GotoX((i*6)+1);
      GLCD_PrintChar(pressed_key);
      GLCD_Render();
      if(i<8){
        student_code[i] = pressed_key;
      }
      i++;
      pressed_key = '\0';
    }
    else if(pressed_key == '#'){
      if((i == STUDENT_CODE_LENGTH) && (student_code[0] == '4') && !std_code_existance(student_code)){
                
                save_student_code_to_eeprom(student_code);
                  
                //lcdcmd(0x01);
                GLCD_Clear();
                
                GLCD_Render();
                //lcdcmd(0x80);
                number_of_students++;
                //lcd_Print("Code Saved!");
                GLCD_GotoLine(0); 
                GLCD_GotoX(0);
                GLCD_PrintString("Code Saved!");
                GLCD_Render();
                _delay_ms(1000); 
                GLCD_Clear();
                GLCD_Render();

            } 
       else if(std_code_existance(student_code)){
            GLCD_Clear();
            GLCD_Render();
                            

            GLCD_GotoLine(0);
            GLCD_GotoX(0);
            GLCD_PrintString("SC already exists");
                
            GLCD_Render();
            _delay_ms(2000);
       }
       else {
      //           lcdcmd(0x01); 
      //           lcdcmd(0x80); 
      //           lcd_Print("Invalid Code!");
                GLCD_Clear();
                GLCD_Render();
                            

                GLCD_GotoLine(0);
                GLCD_GotoX(0);
                GLCD_PrintString("Invalid Code!");
                
                GLCD_Render();
                _delay_ms(1000); 
                PORTD &= ~(1 << BUZZER_PIN);  //  Buzzer on
                _delay_ms(1000);             
                PORTD |= (1 << BUZZER_PIN); //  Buzzer off
                GLCD_Clear();
                GLCD_Render();
      }
      i = 0;
      pressed_key = '\0';
      break;
    }
    else if(pressed_key == '*'){
      i= 0;
      //lcdcmd(0x10);// To go left one character
      GLCD_ClearLine(2);
      GLCD_GotoX(1);
      GLCD_Render();

      pressed_key = '\0';
    }
  }
  GLCD_Clear();
  GLCD_Render();
}


void Attendance_Initialization(){
  
  unsigned char e = 0;
  pressed_key = '\0';

  // lcdcmd(0x01); //clear LCD
	// lcd_Print("1_submit SC");
  // lcdcmd(0xC0); //set the curser to 2nd line
  // lcd_Print("2_Exit");
//------------------------------------------------------------
  GLCD_Clear();
  GLCD_Render();

  GLCD_GotoLine(0);
  GLCD_GotoX(1);
	GLCD_PrintString("1_submit SC");
  GLCD_GotoLine(2);
  GLCD_GotoX(1);
  GLCD_PrintString("2_Exit");
  GLCD_Render();

  Start_15min_Timer();
    //_delay_ms(1000);
    // --------------------------------------------------------
  while(!e && !Times_up){
    if(pressed_key != '\0'){
      switch (pressed_key)
      {
      case '1':
          
          get_std_code();
          
          GLCD_GotoLine(0);
          GLCD_GotoX(1);
          GLCD_PrintString("1_submit SC");
          GLCD_GotoLine(2);
          GLCD_GotoX(1);
          GLCD_PrintString("2_Exit");
          GLCD_Render();
        break;
      case '2':
        e = 1;
        break;
      default:
        break;
      }
    }
  }
  GLCD_Clear();
  GLCD_Render();
  Times_up = 0;
  Disable_Timer_interrupt();
}

void check_std_code_existence(){
  pressed_key = '\0';
  // lcdcmd(0x01); //clear LCD
  // lcdcmd(0x80);//set the curser to first line
  GLCD_Clear();
  GLCD_Render();
  GLCD_GotoLine(0);
  GLCD_GotoX(1);
  GLCD_PrintString("student code:");
  GLCD_Render();
	//lcd_Print("student code:");
  //lcdcmd(0xC0); //set the curser to 2nd line
  GLCD_GotoLine(2);
  GLCD_GotoX(1);
  uint8_t i = 0;

  while(1){
    if((pressed_key != '\0') & (pressed_key != '#')& (pressed_key != '*')){
      //lcd_data(pressed_key);
      GLCD_GotoLine(2);
      GLCD_GotoX((i*6)+1);
      GLCD_PrintChar(pressed_key);
      GLCD_Render();
      if(i<8){
        student_code[i] = pressed_key;
      }
      i++;
      pressed_key = '\0';
    }
    else if(pressed_key == '#'){
      if((i == STUDENT_CODE_LENGTH) & (student_code[0] == '4')){
                if(std_code_existance(student_code)){
                  // lcdcmd(0x01);
                  // lcdcmd(0x80);
                  GLCD_Clear();
                  GLCD_Render();
                  GLCD_GotoLine(0);
                  GLCD_GotoX(1);
                  GLCD_PrintString("Present!");
                  GLCD_Render(); 
                  
                  // lcd_Print("Present!");
                }
                else{
                  // lcdcmd(0x01); 
                  // lcdcmd(0x80);
                  // lcd_Print("Absent!");
                  GLCD_Clear();
                  GLCD_Render();
                  GLCD_GotoLine(0);
                  GLCD_GotoX(1);
                  GLCD_PrintString("Absent!");
                  GLCD_Render(); 
                }
                
                _delay_ms(1500);
            } 
      else {
                // lcdcmd(0x01); 
                // lcdcmd(0x80); 
                GLCD_Clear();
                GLCD_Render();
                GLCD_GotoLine(0);
                GLCD_GotoX(1);
                GLCD_PrintString("Invalid Code!");
                GLCD_Render(); 
                // lcd_Print("Invalid Code!");
                PORTD &= ~(1 << BUZZER_PIN);  //  Buzzer on
                _delay_ms(1000);             
                PORTD |= (1 << BUZZER_PIN); //  Buzzer off
      }
      i = 0;
      pressed_key = '\0';
      break;
    }
    else if(pressed_key == '*'){
      i = 0;
      //lcdcmd(0x10);
      GLCD_ClearLine(2);
      GLCD_GotoX(1);
      GLCD_Render();
      pressed_key = '\0';
    }
  }
  GLCD_Clear();
  GLCD_Render();
}


void Student_Management(){
  unsigned char e = 0;
  pressed_key = '\0';
// ---------------------------------------------------
        GLCD_Clear();
        GLCD_GotoLine(0);
        GLCD_GotoX(1);
	      GLCD_PrintString("1_check SC");
        GLCD_GotoLine(1);
        GLCD_GotoX(1);
        GLCD_PrintString("2_Exit");
        //GLCD_PrintString(adc_str);
        GLCD_Render();
        _delay_ms(1000);
//------------------------------------------------------
  // lcdcmd(0x01); //clear LCD
	// lcd_Print("1_check SC");
  // lcdcmd(0xC0); //set the curser to 2nd line
  // lcd_Print("2_Exit");

  while(!e){
    if(pressed_key != '\0'){
      switch (pressed_key)
      {
      case '1':
        check_std_code_existence();
        
        // lcdcmd(0x01); //clear LCD
        // lcd_Print("1_check SC");
        // lcdcmd(0xC0); //set the curser to 2nd line
        // lcd_Print("2_Exit");
        GLCD_Clear();
        GLCD_GotoLine(0);
        GLCD_GotoX(1);
	      GLCD_PrintString("1_check SC");
        GLCD_GotoLine(1);
        GLCD_GotoX(1);
        GLCD_PrintString("2_Exit");
        GLCD_Render();
        _delay_ms(1000);
        break;
      case '2':
        e = 1;
        break;
      default:
        break;
      }
    }
  }
  GLCD_Clear();
  GLCD_Render();
}
void Temperature_Monitoring() {
    pressed_key = '\0';
    ADC_Init();
    GLCD_Clear();
    GLCD_Render();
    while (pressed_key == '\0') {
        int16_t adc_value = ADC_Read(0); 

        char adc_str[16];
        snprintf(adc_str, sizeof(adc_str), "ADC: %d", adc_value);
        
        
        GLCD_Clear();

        _delay_ms(1000);
        // lcdcmd(0x01); // clear LCD
        //lcd_Print(adc_str);
        GLCD_GotoLine(0);
        GLCD_GotoX(1);
        GLCD_PrintString("temperature:");
        GLCD_GotoLine(1);
        GLCD_GotoX(1);
	      GLCD_PrintString(adc_str);
        //GLCD_PrintString(adc_str);
        GLCD_Render();
        _delay_ms(1000);
    }
    GLCD_Clear();
    GLCD_Render();

    ADC_end();
}

int main(){
  
    DDRC = 0xFF;  // Set PORTC as Output (Data)
    DDRD = 0xFF;  // Set PORTD as Output (Control)

    DDRD |= (1<<BUZZER_PIN); // setting buzzer
    PORTD |= (1 << BUZZER_PIN);


    GLCD_Setup();
	  GLCD_Clear();

    GLCD_SetFont(Font5x8, 5, 8, GLCD_Merge);


    //lcd_init();   // Initialize LCD
    setupKeyPad(); // Initialize Keypad

    while (1) {
      while(pressed_key == '\0'){
        start_showing();
      }
    
      
      switch(pressed_key){
        case('1'):
          Attendance_Initialization();
          break;
        case('2'):
          Student_Management();
          break;
        case('3'):
          read_student_codes_from_eeprom();
          break;
        case('4'):
          Temperature_Monitoring();
          break;
        case('5'):
          EEPROM_VirtualPrint_stdID();
          break;
        case('6'):
        // Monitor traffic
          Ultrasoinc();
          break;
        case('7'):
          del_std_code();
          break;
        default:
          //Attendance_Initialization();
          break;
      }
      pressed_key = '\0';

    }

  return 0;
}