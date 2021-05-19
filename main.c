#define _XTAL_FREQ 4000000

#include <xc.h>
#include "lcd.h"
#include "util.h"

DoorStatus doorStatus = 4;

void main(void)
{
	LCD_init();
	LCD_config();
	
	interrupt_config();
	
	doorController_init();
	
	
	while (1);
	
	
	return;
}

/**
 * Routine of interrupt module configuration.
 */
void interrupt_config(void)
{
	/* RCON: RESET CONTROL REGISTER */
	RCONbits.IPEN = 1; // enable priority levels on interrupts
	
	/* INTCON: INTERRUPT CONTROL REGISTER */
	INTCONbits.GIEH = 1;	// when IPEN = 1, enables all high-priority interrupts
	INTCONbits.GIEL = 0; // when IPEN = 1, disables all low-priority peripheral interrupts
	
	/* INTCONx: INTERRUPT CONTROL REGISTER x */
	INTCONbits.INT0IF = 0;
	INTCONbits.INT0IE = 1;
	
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
	if (INTCONbits.INT0IE && INTCONbits.INT0IF) {	// Testes
		
		switch (doorStatus) {
			case closed: doorController_openingDoor(); break;
			case opened: doorController_closingDoor(); break;
			default:;
		}
		
		INTCONbits.INT0IF = 0;
	}
	
	
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
		
		if (doorStatus == opened) { // se a porta estiver aberta e o alarme de incêndio estiver desligado
			doorController_closingDoor();
		}
		INTCON3bits.INT2IF = 0;
	}
	
}

void doorController_init()
{
	LCD_clearScreen();
	
	/* Inicialização do controlador.
	 * Tempo = (32 letras * 32 ms de delay) + 976 ms = 1024ms + 976ms = 2000ms = 2s
	  */
	LCD_placeCursorInPosition(0x81);
	LCD_print("Inicializando", 32);
	LCD_placeCursorInPosition(0x92);
	LCD_print("CONTROLADOR", 32);
	LCD_placeCursorInPosition(0xD2);
	LCD_print("DA PORTA", 32);
	__delay_ms(976);
	
	
	LCD_clearScreen();
	LCD_placeCursorInPosition(0x81);
	LCD_print("Status:", 10);
	LCD_placeCursorInPosition(0xC3);
	LCD_print("Fechada    ", 10);
	doorStatus = closed;
	
}

void doorController_openingDoor()
{
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

void doorController_closingDoor()
{
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