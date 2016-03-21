/*
 * AsuroWASD.cpp
 *
 * Created: 04.05.2015 11:54:28
 *  Author: Nico
 */ 


//#include <avr/io.h>
#include "asuro.h"
#include "Uart.h"


#define links 0
#define rechts 1

void setMotor(char DirLeft, char DirRight, char SpeedLeft, char SpeedRight); 



int main(void)
{
	Init(); 
	char ReadData[15];
	char SpeedLeft = 200;
	char SpeedRight = 200;
	char DirRight = FREE; 
	char DirLeft = FREE; 
	usart_init_intr(9600);
	uint32_t handshake = 0; 
	uint32_t counter = 0; 
	uint32_t speedcounter = 1; 
	int i=0; 
	
	
    while(1)
    {
//-----------------------------------------------------------------Einlesen
		if (usart_byte_avail_intr() == 1)
		{
			ReadData[0] = (unsigned char)usart_getc_intr();
			//usart_putc(ReadData[0]); 
			if ((ReadData[0] == 'G') /*&& (usart_string_avail_intr() == 1)*/)
			{
				StatusLED(OFF); 
				usart_puts(" IF_G ");
				usart_putc(ReadData[0]);
				i=1; 
				while (ReadData[i] != 'E' && usart_byte_avail_intr() == 1)
				{
					ReadData[i]=usart_getc_intr();
					//usart_putc(ReadData[i]);  
					i++;
					StatusLED(GREEN); 
					usart_putc('M'); 
				}
				
				usart_putc('N'); 
			}
			//usart_puts("Asuro: " + ReadData[0]);
		}
//-----------------------------------------------------------------WASD Steuerung
//-----------------------------------------------vorwärts
		if(ReadData[0] == 'W')
		{
			DirLeft = FWD;
			DirRight = FWD;
			BackLED(ON,ON);
			FrontLED(OFF);	
			//usart_putc(ReadData[0]); 			 	
		}
//----------------------------------------------rückwärts
		else if (ReadData[0] == 'S')
		{
			DirLeft = RWD;
			DirRight = RWD;
			BackLED(ON, ON);
			FrontLED(ON);
			//usart_putc(ReadData[0]);
		}
//-----------------------------------------------links
		else if (ReadData[0] == 'A')
		{
			DirLeft = FREE;
			DirRight = FWD;
			BackLED(ON,OFF);
			FrontLED(OFF);
						usart_putc('L');
						usart_puti((int) SpeedLeft, 3);
			//usart_putc(ReadData[0]);
		}
//-----------------------------------------------rechts
		else if (ReadData[0] == 'D')
		{
			DirLeft = FWD;
			DirRight = FREE;
			BackLED(OFF, ON);
			FrontLED(OFF);
			usart_putc('R');
			usart_puti((int) SpeedRight, 3);
			//usart_putc(ReadData[0]);
		}
//-----------------------------------------------Speed
		else if (ReadData[0] == 'G')
		{
			SpeedLeft  = (char) atol(ReadData +1);
			i=1; 
			while (ReadData[i] != 'Z')
			{
				i++; 
			}
			
			SpeedRight = (char) atol(ReadData +i+1); 			
			
			usart_putc(ReadData[0]); 
			usart_putc('L'); 
			usart_puti((int) SpeedLeft, 3);
			usart_putc('R');
			usart_puti((int) SpeedRight, 3);

		}
//------------------------------------------------keine Zeichen empfangen
		else if (ReadData[0] == 'X')
		{
			DirLeft = FREE;
			DirRight = FREE;
			FrontLED(OFF);
			BackLED(OFF, OFF);
			//StatusLED(YELLOW);
		}
//-----------------------------------------------Handshake
		else if (ReadData[0] == 'h')
		{
			handshake = 1; 				
		}
		
		if (counter < 100000)
		{
			counter++;
		}
		else
		{
			counter=0; 
			ReadData[0] = 'X';
		}
		
		
		//ReadData[0] = 'X';//---empfangene Zeichen zurücksetzen
		if (SpeedLeft < 85) {
			SpeedLeft = 85; 
		}
		if (SpeedRight < 85){
			SpeedRight =85; 
		}
		setMotor(DirLeft, DirRight, SpeedLeft, SpeedRight);
		
//---------------------------------------------------------Handshake 
		if (handshake == 1)
		{
			usart_puts("Handshake ");
			handshake = 0;  
		}
	
	}//--------------------------------------------------Ende while
}//-------------------------------------------------Ende main



void setMotor(char DirLeft, char DirRight, char SpeedLeft, char SpeedRight)
{
	MotorDir(DirLeft,DirRight);
	MotorSpeed(SpeedLeft, SpeedRight);
	
	return;  
}

