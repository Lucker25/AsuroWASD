#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>


void usart_init_intr(uint16_t baud); //Initialisierung

uint8_t usart_byte_avail_intr(void); // Byte verfügbar
//--------------------------------------------------------------
uint8_t usart_string_avail_intr(void);// string verfügbar
//--------------------------------------------------------------
uint8_t usart_getc_intr(void); //Byte Auslesen
//--------------------------------------------------------------
uint8_t usart_gets_intr(char *s); //string auslesen

//=============================================================
//Die nachfolgenden Sendefunktionen sind nicht interrupt-gesteuert
//=============================================================
void usart_putc(uint8_t byte);
//--------------------------------------------------------------
void usart_puts(char *s);
//---------------------------------------------------------------
void usart_puti(int zahl, int sges);
//--------------------------------------------------------------
void usart_putf(float zahl, int sges, int snach);
//--------------------------------------------------------------
void usart_putui(unsigned int zahl, int sges);
//--------------------------------------------------------------
void usart_putui_0(unsigned int zahl, int sges);
//--------------------------------------------------------------
void usart_putui_hex(unsigned int zahl, int sges);
//--------------------------------------------------------------
void usart_putui_bin(unsigned int zahl, int sges);
//--------------------------------------------------------------
void usart_putb(uint8_t a);
