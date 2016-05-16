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
	char DirRight = FREE;//FREE
	char DirLeft = FREE;//FREE
	uint16_t IstSpeedLeft = 0;
	uint16_t IstSpeedRight = 0;
	unsigned int  brightnessdata[2] ={0,0};
	uint8_t flag [2] = {0, 0};
	uint8_t rpm [2] = {0, 0};
	int16_t rpmdif = 0;
	uint8_t counter = 0;
	int8_t rpm_auswertung [2] = {0,0};
	unsigned int  odd_data_check[2] ={0,0};	
	float rpm_i = 0; 
	float KP = 2; 
	float KI =0.5; 
	float rpm_p = 0; 
	int16_t rpm_sum = 0; 
	


void setMotor(char DirLeft, char DirRight, unsigned char SpeedLeft, unsigned char SpeedRight); 
void checkData(); 
void readData(); 
int constrain(int variable, int low_limit, int high_limit);

int main(void)
{
	Init();

	usart_init_intr(9600);
	// Timer 0 konfigurieren
	TCCR0 = (1<<CS01); //Prescaler 8 | (1<<CS00); // Prescaler 64
	
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
		
		
		usart_putc('L');
		usart_puti(rpm_auswertung[LEFT], 3);
		usart_putc('R');
		usart_puti(rpm_auswertung[RIGHT], 3);
		usart_puts("\r\n");
	
//-----------------------------------------------------------------Minimal/Maximalwerte
		IstSpeedLeft = constrain(IstSpeedLeft, 100, 255); 
		IstSpeedRight = constrain(IstSpeedRight, 100, 255); 

		setMotor(DirLeft, DirRight, IstSpeedLeft, IstSpeedRight);
		
	
	}
	//------------------------------------------------------------------------------------------------Ende while
}
//----------------------------------------------------------------------------------------------------Ende main



void setMotor(char DirLeft,  char DirRight, unsigned char SpeedLeft, unsigned char SpeedRight)
{
	/*usart_puts("setMotor");
	usart_putc('L');
	usart_puti((int) SpeedLeft, 3);
	usart_putc('R');
	usart_puti((int) SpeedRight, 3);*/
	MotorDir(DirLeft,DirRight);
	//usart_puts("\r\n");
	MotorSpeed(SpeedLeft, SpeedRight);
	
	
	return;  
}

void checkData() {
	//-----------------------------------------------vorwärts
	if(ReadData[0] == 'W')
	{
		DirLeft = FWD;
		DirRight = FWD;
		//BackLED(ON,ON);
		//FrontLED(OFF);
		//usart_putc(ReadData[0]);
	}
	//----------------------------------------------rückwärts
	else if (ReadData[0] == 'S')
	{
		DirLeft = RWD;
		DirRight = RWD;
		//BackLED(ON, ON);
		//FrontLED(ON);
		//usart_putc(ReadData[0]);
	}
	//-----------------------------------------------links
	else if (ReadData[0] == 'A')
	{
		DirLeft = FREE;
		DirRight = FWD;
		//BackLED(ON,OFF);
		//FrontLED(OFF);
		/*usart_putc('L');
		usart_puti((int) SollSpeedLeft, 3);*/
		//usart_putc(ReadData[0]);
	}
	//-----------------------------------------------rechts
	else if (ReadData[0] == 'D')
	{
		DirLeft = FWD;
		DirRight = FREE;
		//BackLED(OFF, ON);
		//FrontLED(OFF);
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
  (1000000/8)/256 Hz = 488 Hz
  bzw.
  1/488 s = 0,002 s  = 2 ms
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
  if (counter >= 75) // 150 ms 
  {	
	  counter = 0; 
	  rpm_auswertung[LEFT] = rpm[LEFT]; 
	  rpm_auswertung[RIGHT] = rpm[RIGHT]; 
	  rpm[LEFT]= 0;
	  rpm[RIGHT] = 0; 
  
  
	rpmdif = rpm_auswertung[LEFT] - rpm_auswertung[RIGHT];
  
  
  
	rpm_p = rpmdif * KP;
  
	rpm_sum = rpm_sum + rpmdif;
	rpm_sum = constrain(rpm_sum, -255, 255);
	rpm_i = rpm_sum * 0;
  
  
	IstSpeedRight = IstSpeedRight + (rpm_i + rpm_p);
	IstSpeedRight = constrain(IstSpeedRight, 100, 255); 
	
	MotorSpeed(IstSpeedLeft, IstSpeedRight);
	StatusLED(GREEN);
  }
  StatusLED(RED); 
}

int constrain(int variable, int low_limit, int high_limit){
	if (variable < low_limit){
		return low_limit; 
	}
	else if (variable > high_limit){
		return high_limit; 
	}
	else {
		return variable; 
	}
	
	
}