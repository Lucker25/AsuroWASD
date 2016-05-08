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
	unsigned int  brightnessdata[2] ={0,0};
	uint8_t flag [2] = {0, 0};
	uint8_t rpm [2] = {0, 0};
	int16_t rpmdif = 0;
	uint8_t counter = 0;
	uint16_t rpm_auswertung [2] = {0,0};
	


void setMotor(char DirLeft, char DirRight, char SpeedLeft, char SpeedRight); 
void checkData(); 
void readData(); 

int main(void)
{
	Init();

	usart_init_intr(9600);
	// Timer 0 konfigurieren
	TCCR0 = (1<<CS01) | (1<<CS00); // Prescaler 64
	
	// Overflow Interrupt erlauben
	TIMSK |= (1<<TOIE0);
	
	// Global Interrupts aktivieren
	sei();

	
	
	
    while(1)
    {
//-----------------------------------------------------------------Einlesen
		readData(); 
//-----------------------------------------------------------------WASD Steuerung
		checkData(); 
//-----------------------------------------------------------------Regelung
		

		
		rpmdif = rpm_auswertung[LEFT] - rpm_auswertung[RIGHT];
		usart_puts("RPMDIF ");
		usart_puti(rpmdif, 3);
		usart_puts("\r\n");
		
		
		/*if (rpmdif > 1) {

			IstSpeedRight += 2;

		}
		if (rpmdif < -1 ) {

			IstSpeedRight -= 2;


		}*/
	
//-----------------------------------------------------------------Minimaldrehzahl
		if (IstSpeedLeft < 100) {
			IstSpeedLeft = 100; 
		}
		if (IstSpeedRight < 100){
			IstSpeedRight =100; 
		}
		if (IstSpeedLeft > 255) {
			IstSpeedLeft = 255;
		}
		if (IstSpeedRight > 255){
			IstSpeedRight = 255;
		}
		setMotor(DirLeft, DirRight, IstSpeedLeft, IstSpeedRight);
		
//---------------------------------------------------------Handshake 
		if (handshake == 1)
		{
			usart_puts("Handshake ");
			usart_puts("\r\n");
			handshake = 0;  
		}
	
	}
	//------------------------------------------------------------------------------------------------Ende while
}
//----------------------------------------------------------------------------------------------------Ende main



void setMotor(char DirLeft, char DirRight, char SpeedLeft, char SpeedRight)
{
	/*usart_puts("setMotor");
	usart_putc('L');
	usart_puti((int) SpeedLeft, 3);
	usart_putc('R');
	usart_puti((int) SpeedRight, 3);*/
	MotorDir(DirLeft,DirRight);
	//usart_puts("\r\n");
	MotorSpeed(220, 200);
	
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
		/*usart_putc('L');
		usart_puti((int) SollSpeedLeft, 3);*/
		//usart_putc(ReadData[0]);
	}
	//-----------------------------------------------rechts
	else if (ReadData[0] == 'D')
	{
		DirLeft = FWD;
		DirRight = FREE;
		BackLED(OFF, ON);
		FrontLED(OFF);
		/*usart_putc('R');
		usart_puti((int) SollSpeedRight, 3);*/
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
		usart_puts("Handshake \r\n");
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
					//usart_puts(" IF_G \r\n");
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


ISR (TIMER0_OVF_vect)
{
  /* Interrupt Aktion alle
  (1000000/64)/256 Hz = 61 Hz
  bzw.
  1/256 s = 0,016 s  = 16 ms
  */

  OdometrieData(brightnessdata);

  if ((brightnessdata[LEFT] < 680) && (flag[LEFT] == BLACK)) {// WEISS
	  flag[LEFT] = WHITE;
	  rpm[LEFT]++;
	  
  }
  
  if ((brightnessdata[LEFT] > 720) && (flag[LEFT] == WHITE)) {// SCHWARZ
	  flag[LEFT] = BLACK;
	  rpm[LEFT]++; 
	  
  }
  
  if ((brightnessdata[RIGHT] < 680) && (flag[RIGHT] == BLACK)) {// WEISS
	  flag[RIGHT] = WHITE;
	  rpm[RIGHT]++;
	  
  }
  
  if ((brightnessdata[RIGHT] > 720) && (flag[RIGHT] == WHITE)) {// SCHWARZ
	  flag[RIGHT] = BLACK;
	  rpm[RIGHT]++;
	  
  }
  counter++; 
  if (counter == 31) // ca 1/2 Sekunde 
  {	
	  counter = 0; 
	  rpm_auswertung[LEFT] = rpm[LEFT]; 
	  rpm_auswertung[RIGHT] = rpm[RIGHT]; 
	  rpm[LEFT]= 0;
	  rpm[RIGHT] = 0; 
  }
  
}