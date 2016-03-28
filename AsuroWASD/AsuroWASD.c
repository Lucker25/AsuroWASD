/*
 * AsuroWASD.cpp
 *
 * Created: 04.05.2015 11:54:28
 *  Author: Nico
 */ 


//#include <avr/io.h>
#include "asuro.h"
#include "Uart.h"



//--------------------------------------------------Variablen
	char ReadData[15];
	uint32_t SollSpeedLeft = 200;
	uint32_t SollSpeedRight = 200;
	char DirRight = FREE;
	char DirLeft = FREE;
	uint32_t handshake = 0;
	uint32_t IstSpeedLeft = 0; 
	uint32_t IstSpeedRight = 0; 

	


void setMotor(char DirLeft, char DirRight, char SpeedLeft, char SpeedRight); 
void checkData(); 
void readData(); 
void readOdometrie(uint8_t side, uint8_t *flag, uint8_t *rpm, unsigned int *brightness[]);


int main(void)
{
	Init(); 

	usart_init_intr(9600);
	uint32_t counter = 0; 
	uint8_t flagLeft = FALSE;
	uint8_t flagRight = FALSE;
	uint8_t rpmLeft = 0;
	uint8_t rpmRight = 0; 
	uint8_t rpmdif = 000; 
	unsigned int  brightnessdata[2];

	
	
	
    while(1)
    {
//-----------------------------------------------------------------Einlesen
		readData(); 
//-----------------------------------------------------------------WASD Steuerung
		checkData(); 
//-----------------------------------------------------------------Regelung
		readOdometrie(LEFT, flagLeft, rpmLeft, brightnessdata); 
		readOdometrie(RIGHT, flagRight, rpmRight, brightnessdata); 
		
		rpmdif = rpmLeft - rpmRight; 
		usart_puts("RPM");
		usart_puti((int) rpmdif, 3);
		
		/*if (rpmdif > 0) {
			IstSpeedLeft--;
		}
		else if (rpmdif < 0) {
			IstSpeedRight--; 
		}
		else{
			IstSpeedLeft = SollSpeedLeft; 
			IstSpeedRight = SollSpeedRight;
			rpmLeft = 0; 
			rpmRight = 0;  
		}		*/
//-----------------------------------------------------------------Minimaldrehzahl
		if (IstSpeedLeft < 100) {
			IstSpeedLeft = 100; 
		}
		if (IstSpeedRight < 100){
			IstSpeedRight =100; 
		}
		setMotor(DirLeft, DirRight, IstSpeedLeft, IstSpeedRight);
		
//---------------------------------------------------------Handshake 
		if (handshake == 1)
		{
			usart_puts("Handshake ");
			handshake = 0;  
		}
	
	}
	//------------------------------------------------------------------------------------------------Ende while
}
//----------------------------------------------------------------------------------------------------Ende main



void setMotor(char DirLeft, char DirRight, char SpeedLeft, char SpeedRight)
{
	usart_putc('M');
	usart_putc('L');
	usart_puti((int) SpeedLeft, 3);
	usart_putc('R');
	usart_puti((int) SpeedRight, 3);
	MotorDir(DirLeft,DirRight);
	MotorSpeed(SpeedLeft, SpeedRight);
	
	return;  
}

void checkData() {
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
		usart_puti((int) SollSpeedLeft, 3);
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
		usart_puti((int) SollSpeedRight, 3);
		//usart_putc(ReadData[0]);
	}
	//-----------------------------------------------Speed
	else if (ReadData[0] == 'G')
	{
		SollSpeedLeft  = (char) atol(ReadData +1);
		int i=1;
		while (ReadData[i] != 'Z')
		{
			i++;
		}
		
		SollSpeedRight = (char) atol(ReadData +i+1);
		IstSpeedLeft = SollSpeedLeft; 
		IstSpeedRight = SollSpeedRight; 
		
		/*usart_putc(ReadData[0]);
		usart_putc('L');
		usart_puti((int) SollSpeedLeft, 3);
		usart_putc('R');
		usart_puti((int) SollSpeedRight, 3);*/

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
		usart_puts("Handshake ");
	}
}

void readData(){
			if (usart_byte_avail_intr() == 1)
			{
				ReadData[0] = (unsigned char)usart_getc_intr();
				//usart_putc(ReadData[0]);
				if ((ReadData[0] == 'G') /*&& (usart_string_avail_intr() == 1)*/)
				{
					StatusLED(OFF);
					//usart_puts(" IF_G ");
					//usart_putc(ReadData[0]);
					int i=1;
					while (ReadData[i] != 'E' && usart_byte_avail_intr() == 1)
					{
						ReadData[i]=usart_getc_intr();
						//usart_putc(ReadData[i]);
						i++;
						StatusLED(GREEN);
						//usart_putc('M');
					}
					
					//usart_putc('N');
				}
				//usart_puts("Asuro: " + ReadData[0]);
			}
}

void readOdometrie(uint8_t side,uint8_t *flag, uint8_t *rpm, unsigned int *brightness[]){
	
	OdometrieData(brightness); 
	usart_putc('B');
	if (side == LEFT){
		usart_puti((int) brightness[LEFT], 3);

		if ((brightness[LEFT] < 550) && (flag == TRUE)) {
			
			flag = FALSE; 
			rpm++;
		
		}
	
		if ((brightness[LEFT] > 650) && (flag == FALSE)) {
			
			flag = TRUE; 
			rpm++;
		
		}
	}
	if (side == RIGHT){
		usart_puti((int) brightness[RIGHT], 3);
			if ((brightness[RIGHT] < 550) && (flag == TRUE)) {
			
			flag = FALSE;
			rpm++;
			
		}
		
		if ((brightness[RIGHT] > 650) && (flag == FALSE)) {
			
			flag = TRUE;
			rpm++;
			
		}
	}
}