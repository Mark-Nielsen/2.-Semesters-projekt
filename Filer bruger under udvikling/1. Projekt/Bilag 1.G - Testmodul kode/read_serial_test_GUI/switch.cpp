/*
 * switch.cpp
 *
 * Created: 08-12-2025 10:01:35
 *  Author: perni
 */ 
#include <avr/io.h>

// Klarg�r switch-porten
void initSwitchPort()
{
	DDRA = 0x00;			//input
	PORTA = 0xFF; 
}

// L�ser alle switches samtidigt
unsigned char switchStatus()
{
	unsigned char status;
	status = ~PINA;
	return status;
}

// Returnerer TRUE, hvis switchen med nummeret
// "switch_nr" er aktiveret - ellers returneres FALSE
unsigned char switchOn(unsigned char switch_nr)
{
	unsigned char mask;
	mask = 0b00000001<< switch_nr;
	return ((mask & PINA) == 0);

}