/**
 * Universidade Federal da Grande Dourados - UFGD
 * Faculdade de Ciências Exatas e Tecnologia - FACET
 * Curso de Bacharelado em Engenharia de Computação - BEC
 *
 * Disciplina: MICROCONTROLADORES E APLICAÇÕES
 * Prof.: Marcos Mansano Furlan
 * Período: 2020.01 RAEMF (2021.01)
 *
 * Projeto Final: Sistema Controlador de Porta
 *
 * Desenvolvido por: Filipe Messias,
 *						   Jeferson Willian Vieira Silva, e
 *							João Vitor Nascimento de Souza.
 * Desenvolvido em: 19, 26 e 27 de maio de 2021.
 */

#include <xc.h>
#include "util.h"

/**
 * Procedimento que inicializa os recursos necessários.
 */
void init(void)
{
	LCD_init();
	ADC_init();
	interrupt_init();
	doorController_init();
	
	CLRWDT();
}

/**
 * Procedimento principal: ponto de entrada da aplicação.
 */
void main(void)
{	
	init();
	
	double value = 0.0;				// leitura atual do sensor
	double previousValue = 0.0;	// leitura anterior do sensor
	double distance = 0.0;			// distância de alguém até a porta
	
	while (1) 
	{
		if (doorStatus != fire && doorStatus != invasion) {
		// Se não há alarmes de incêndio ou de invasão ativados
			
			ADCON0bits.GO = 1;
			while (!ADCON0bits.DONE);
			//CLRWDT();
			value = (5.0) * ((ADRESH<<8) + ADRESL) / 1023;

			if (value != previousValue) { // evitar atualização do display quando a leitura é igual a anterior

				distance = 5.0 - value;

				if (distance <= 2.0) { // o sensor está lendo alguém próximo (pelo menos 1,5 m) à porta
					doorController_openingDoor(); // a porta abre se estiver fechada
				} else { // distance > 2.0, o sensor está lendo movimento, mas ainda não está próximo o suficiente para abrir a porta
					doorController_closingDoor();	// a porta fecha se estiver aberta
				}

				LCD_placeCursorInPosition(0x91);
				LCD_print("Movimento:", 10);
				LCD_placeCursorInPosition(0xD3);
				LCD_printFloatNumber(distance, 2);
				LCD_placeCursorInPosition(0xD7);
				LCD_printChar('m');
				
				previousValue = value;
			}
		}
		if (doorStatus != opened) {
			CLRWDT(); // Free Conter WTD.
		}
	}
	
	return;
}