#define _XTAL_FREQ 4000000

#include <xc.h>
#include "lcd.h"
#include "util.h"

DoorStatus doorStatus = 4;

void main(void)
{
	LCD_init();
	ADC_init();
	interrupt_init();
	
	doorController_init();
	
	double value = 0.0;				// leitura atual do sensor
	double previousValue = 0.0;	// leitura anterior do sensor
	
	double distance = 0.0;			// distância de alguém até a porta
	
	while (1) 
	{
		if (doorStatus != fire && doorStatus != invasion) {
			
			ADCON0bits.GO = 1;
			while (!ADCON0bits.DONE);
			value = (5.0) * ((ADRESH<<8) + ADRESL) / 1023;

			if (value != previousValue) { // evitar atualização do display quando a leitura é igual a anterior

				distance = 5.0 - value;

				if (distance <= 2.0) {
				// o sensor está lendo alguém próximo (pelo menos 1,5 m) à porta
					doorController_openingDoor(); // a porta abre se estiver fechada
				} else { // distance > 2.0
				// o sensor está lendo movimento, mas ainda não está próximo o suficiente para abrir a porta
					doorController_closingDoor();	// a porta fecha se estiver aberta
				}

				// DEBUG
				LCD_placeCursorInPosition(0xD1);
				LCD_printFloatNumber(value, 2);
				previousValue = value;
			}
		}
		
	}
	
	return;
}

/**
 * Routine of interrupt module configuration.
 */
void interrupt_init(void)
{
	/* RCON: RESET CONTROL REGISTER */
	RCONbits.IPEN = 1; // enable priority levels on interrupts
	
	/* INTCON: INTERRUPT CONTROL REGISTER */
	INTCONbits.GIEH = 1;	// when IPEN = 1, enables all high-priority interrupts
	INTCONbits.GIEL = 0; // when IPEN = 1, disables all low-priority peripheral interrupts
	
	INTCON3bits.INT1F = 0;	// the INT1 external interrupt did not occur
	INTCON3bits.INT1IP = 1;	// high priority
	INTCON2bits.INTEDG1 = 0;// interrupt on falling edge
	INTCON3bits.INT1IE = 1;	// enables the INT2 external interrupt 
	
	INTCON3bits.INT2IF = 0;
	INTCON3bits.INT2IP = 1;
	INTCON2bits.INTEDG2 = 0;
	INTCON3bits.INT2IE = 1;
	
}

/**
 * Routine for handling interrupts.
 */
void __interrupt(high_priority) interrupt_service_routine(void)
{	
	if (INTCON3bits.INT1IE && INTCON3bits.INT1IF) {	// Incêndio
	
		switch (doorStatus) {
			case closed:
			case invasion:	doorController_openingDoor();
			case opened:	doorController_fire(); break;
			case fire:		doorController_closingDoor();		
		}
		
		INTCON3bits.INT1IF = 0;
	}
	
	
	if (INTCON3bits.INT2IE && INTCON3bits.INT2IF) { // Invasão
		
		switch (doorStatus) {
			case opened: doorController_closingDoor();
			case closed: doorController_invasion(); break;
			case invasion: doorController_openingDoor();
			case fire:;
		}
		
		INTCON3bits.INT2IF = 0;
	}
	
}

void ADC_init() {
	
	TRISAbits.RA1 = 1;	// entrada do sinal analógico
	
	ADCON0 = 0b00000000;	// canal 0 com conversor desativado
	ADCON1 = 0b00001110;	// configura tensão referência interna
	ADCON2 = 0b10111110;	// configura AD com 20TAD Fosc/64
	
	ADCON0bits.ADON = 1; // Ativa o conversor
}


void doorController_init()
{
	LCD_clearScreen();
	
	/* Inicialização do controlador.
	 * Tempo = (32 letras * 64 ms de delay) + 952 ms = 1024ms + 952ms = 3000ms = 3s
	 */
	LCD_placeCursorInPosition(0x81);
	LCD_print("Inicializando", 64);
	LCD_placeCursorInPosition(0x92);
	LCD_print("CONTROLADOR", 64);
	LCD_placeCursorInPosition(0xD2);
	LCD_print("DA PORTA", 64);
	__delay_ms(952);
	
	LCD_clearScreen();
	LCD_placeCursorInPosition(0x81);
	LCD_print("Status:", 10);
	LCD_placeCursorInPosition(0xC3);
	LCD_print("Fechada    ", 10);
	doorStatus = closed;
	
}

void doorController_openingDoor()
{
	if (doorStatus != opened) { // se a porta não estiver aberta, abrir.
		LCD_clearScreen();
		LCD_placeCursorInPosition(0x81);
		LCD_print("Status:", 50);

		LCD_placeCursorInPosition(0xC3);
		LCD_print("Abrindo...", 10);
		LCD_customDelay_ms(1500);	// a porta gasta 1,5 s para abrir

		LCD_placeCursorInPosition(0xC3);
		LCD_print("Aberta    ", 10);
		
		doorStatus = opened;
	}
}

void doorController_closingDoor()
{
	if (doorStatus != closed) {	// se a porta não estiver fechada, fechar.
		LCD_clearScreen();
		LCD_placeCursorInPosition(0x81);
		LCD_print("Status:", 10);

		LCD_placeCursorInPosition(0xC3);
		LCD_print("Fechando...", 10);
		LCD_customDelay_ms(1500);	// a porta gasta 1,5 s para fechar

		LCD_placeCursorInPosition(0xC3);
		LCD_print("Fechada    ", 10);

		doorStatus = closed;
	}
}

void doorController_fire()
{
	LCD_clearScreen();
	LCD_placeCursorInPosition(0xC1);
	LCD_print("INCENDIO!!", 10);
	
	LCD_placeCursorInPosition(0x91);
	LCD_print("Emergencia", 10);
	
	doorStatus = fire;
}

void doorController_invasion()
{
	LCD_clearScreen();
	LCD_placeCursorInPosition(0xC1);
	LCD_print("INVASAO!!", 10);
	
	LCD_placeCursorInPosition(0x91);
	LCD_print("Emergencia", 10);
	
	doorStatus = invasion;
}