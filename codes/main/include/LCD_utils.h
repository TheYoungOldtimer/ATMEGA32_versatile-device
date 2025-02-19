#pragma once

#include <avr/io.h>
#include <util/delay.h>
#include "KS0108.h"
#include <Font5x8.h>

#define DATA_PORT PORTC
#define CTRL_PORT PORTD

#define LCD PORTC
#define EN PD3
#define RS PD0




void lcd_enable() {
    PORTD|= (1<<EN); //EN=1 for H to L pulse
	_delay_ms(5);
	PORTD &= ~(1<<EN); // EN=0 for H to L pulse
}

void lcdcmd(unsigned char cmd){
	PORTD &=~(1<<RS); //RS=0 for command (1111 1101)
	
	LCD= cmd;  //Send command
	lcd_enable();
}

// Send Data to LCD
void lcd_data(unsigned char data) {
    PORTD |= (1<<RS); //RS=1 for data
	LCD= data;  //Send upper nibble
	lcd_enable();

}

void lcd_Print(char*str){
	int i;
	for(i=0;str[i]!=0;i++)		/* Send each char of string till the NULL */
	{
		lcd_data(str[i]);
	}
}


void lcd_init(){
	DDRC= 0xFF; // Define  output LCD port(ALL c ports are outputs) 
	DDRD = 0xFF; //Define EN and RS pin  as output

	PORTD &= ~(1<<EN); //initialize EN=0
	lcdcmd(0x38);
	lcdcmd(0x0E);  //display on cursor blinking

}

void start_showing(){
    //lcdcmd(0x01); //clear LCD
	//lcd_Print("Options:");

    //GLCD_Setup();
	//GLCD_Clear();
    GLCD_SetFont(Font5x8, 5, 8, GLCD_Merge);
    
    GLCD_GotoLine(0);
	GLCD_PrintString("Options:");


    GLCD_GotoLine(1);
    GLCD_GotoX(1);
	GLCD_PrintString("1_Attendance Init");

	GLCD_GotoLine(2);
    GLCD_GotoX(1);
	GLCD_PrintString("2_Student Management");
    // lcdcmd(0xC0); //set the curser to 2nd line
    // lcd_Print("1_AttendanceInit");

    // _delay_ms(1500);
    // lcdcmd(0x01); //clear LCD
    // lcdcmd(0x80);//set the curser to first line
    // lcd_Print("2_Std Management");

    GLCD_GotoLine(3);
    GLCD_GotoX(1);
	GLCD_PrintString("3_View Present Std");

	// lcdcmd(0xC0); //set the curser to 2nd line
    // lcd_Print("3_Present Std");
    GLCD_GotoLine(4);
    GLCD_GotoX(1);
	GLCD_PrintString("4_Temperature");

    // _delay_ms(1500);
    // lcdcmd(0x01); //clear LCD
    // lcdcmd(0x80);//set the curser to first line
    // lcd_Print("4_Temperature");

    GLCD_GotoLine(5);
    GLCD_GotoX(1);
	GLCD_PrintString("5_Retrieve Data");


	// lcdcmd(0xC0); //set the curser to 2nd line
    // lcd_Print("5_Retrieve Data");

    GLCD_GotoLine(6);
    GLCD_GotoX(1);
	GLCD_PrintString("6_Monitor Traffic");

    // _delay_ms(1500);
    // lcdcmd(0x01); //clear LCD
    // lcdcmd(0x80);//set the curser to first line
    // lcd_Print("6_Monitor Traffic");
    // _delay_ms(1500);

    GLCD_GotoLine(7);
    GLCD_GotoX(1);
	GLCD_PrintString("7_Delete Student");
    GLCD_Render();



    // _delay_ms(1000);
    // GLCD_Clear();
    // GLCD_Render();
    // _delay_ms(1000);

    
    // lcdcmd(0x01); //clear LCD
    // lcdcmd(0x80);//set the curser to first line
    // lcd_Print("7_Delete Student");
    // _delay_ms(1500);
}