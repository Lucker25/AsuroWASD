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
	unsigned char ReadData[1]={0};
	char SpeedLeft = 100;
	char SpeedRight = 100;
	char DirRight = FWD; 
	char DirLeft = FWD; 
	  
	int i = 0; 
	uint32_t timer;
	usart_init_intr(2400);
	
	
    while(1)
    {
		//----------------------------
		/*if (ReadData[0] == 'x')
		{
			DirLeft = FREE;
			DirRight = FREE;
			SpeedLeft = 0;
			SpeedRight = 0;
			FrontLED(OFF);
			BackLED(OFF, OFF);
			StatusLED(YELLOW);
		}*/
		//-------------------------Einlesen 
		i = 0; 
		timer = 0; 
		StatusLED(GREEN);
		/*while (i < 1 && timer <= 10000) 
		{ 
			if (UCSRA & 0x80)
			{
				ReadData[i++] = UDR;
			}
			timer++;
		}
		utoa( timer, Buffer, 10 );
		SerWrite(Buffer, 12);	*/
		//-------------------------------WASD Steuerung
		if(ReadData[0] == 'w')//vorwärts
		{
			DirLeft = FWD;
			DirRight = FWD;
			BackLED(ON,ON);
			FrontLED(OFF);	
			//SerWrite("W\n\r", 3); 
			//SerWrite("Kein Fehler", 11);			 	
		}
		else if (ReadData[0] == 's')//rückwärts
		{
			DirLeft = RWD;
			DirRight = RWD;
			BackLED(ON, ON);
			FrontLED(ON);
		}
		else if (ReadData[0] == 'a')//links
		{
			DirLeft = FREE;
			DirRight = FWD;
			BackLED(ON,OFF);
			FrontLED(OFF);
		}
		else if (ReadData[0] == 'd')//rechts
		{
			DirLeft = FWD;
			DirRight = FREE;
			BackLED(OFF, ON);
			FrontLED(OFF);
		}
		else if (ReadData[0] == 'i')
		{
			SpeedRight += 2;
			SpeedLeft += 2;
			StatusLED(GREEN); 
		}
		else if (ReadData[0] == 'j')
		{
			SpeedRight -= 2;
			SpeedLeft -= 2;
			StatusLED(RED);
		}
		else if (ReadData[0] == 'q')
		{	
			DirLeft = FREE; 
			DirRight = FREE;
			SpeedLeft = 0; 
			SpeedRight = 0;
			FrontLED(OFF); 
			BackLED(OFF, OFF); 
			StatusLED(YELLOW);   
		}
		else if (ReadData[0] == 'x')
		{
			DirLeft = FREE;
			DirRight = FREE;
			FrontLED(OFF);
			BackLED(OFF, OFF);
			StatusLED(YELLOW);
		}
		
		ReadData[0] = 'x'; 
		//setMotor(DirLeft, DirRight, SpeedLeft, SpeedRight);
		SleepMS(300);
				 
	}
}

void setMotor(char DirLeft, char DirRight, char SpeedLeft, char SpeedRight)
{
	MotorDir(DirLeft,DirRight);
	MotorSpeed(SpeedLeft, SpeedRight);
	
	return;  
}

