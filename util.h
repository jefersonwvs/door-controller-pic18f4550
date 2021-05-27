/**
 * Biblioteca com sub-rotinas para o programa principal.
 */

// PIC18F4550 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1L
#pragma config PLLDIV = 1       // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 1       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator (HS))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = OFF        // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting 2.05V)
#pragma config VREGEN = OFF     // USB Voltage Regulator Enable bit (USB voltage regulator disabled)

// CONFIG2H
#pragma config WDT = ON        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 4096    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = ON      // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = OFF     // Stack Full/Underflow Reset Enable bit (Stack full/underflow will not cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define _XTAL_FREQ 4000000

#include "lcd.h"

typedef enum status {
   fire = 1, // incêndio
   invasion = 2, // invasão
   opened = 3, // porta aberta
   closed = 4 // porta fechada
} DoorStatus;

DoorStatus doorStatus = closed;

void  interrupt_init(void);
void  __interrupt(high_priority) interrupt_service_routine(void);
void  ADC_init(void);

void  doorController_init(void);
void  doorController_openingDoor(void);
void  doorController_closingDoor(void);
void  doorController_fire(void);
void  doorController_invasion(void);

/**
 * Procedimento que iniciliza e configura interrupções.
 */
void interrupt_init(void)
{
   /* RCON: RESET CONTROL REGISTER */
   RCONbits.IPEN = 1; // enable priority levels on interrupts

   /* INTCON: INTERRUPT CONTROL REGISTER */
   INTCONbits.GIEH = 1; // when IPEN = 1, enables all high-priority interrupts
   INTCONbits.GIEL = 0; // when IPEN = 1, disables all low-priority peripheral interrupts

   INTCON3bits.INT1F = 0; // the INT1 external interrupt did not occur
   INTCON3bits.INT1IP = 1; // high priority
   INTCON2bits.INTEDG1 = 0; // interrupt on falling edge
   INTCON3bits.INT1IE = 1; // enables the INT2 external interrupt 

   INTCON3bits.INT2IF = 0;
   INTCON3bits.INT2IP = 1;
   INTCON2bits.INTEDG2 = 0;
   INTCON3bits.INT2IE = 1;
}

/**
 * Procedimento tratador de interrupções.
 */
void __interrupt(high_priority) interrupt_service_routine(void)
{
   if (INTCON3bits.INT1IE && INTCON3bits.INT1IF) {
   // Ativando ou desativando alarme de incêndio
      switch (doorStatus) {
         case closed:
         case invasion: doorController_openingDoor();
         case opened:   doorController_fire();  break;
         case fire: doorController_closingDoor();
      }
      INTCON3bits.INT1IF = 0;
   }


   if (INTCON3bits.INT2IE && INTCON3bits.INT2IF) {
   // Ativando ou desativando alarme de invasão
      switch (doorStatus) {
         case opened: doorController_closingDoor();
         case closed: doorController_invasion();
            break;
         case invasion: doorController_openingDoor();
         case fire:;
      }
      INTCON3bits.INT2IF = 0;
   }
   
   CLRWDT();

}

/**
 * Procedimento que inicializa e configura o módulo conversor A/D.
 */
void ADC_init(void)
{
   TRISAbits.RA1 = 1; // entrada do sinal analógico

   ADCON0 = 0b00000000; // canal 0 com conversor desativado
   ADCON1 = 0b00001110; // configura tensão referência interna
   ADCON2 = 0b10111110; // configura AD com 20TAD Fosc/64

   ADCON0bits.ADON = 1; // Ativa o conversor
}

/**
 * Procedimento que inicializa e configura o controlador de portas.
 */
void doorController_init(void)
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

/**
 * Procedimento para abrir a porta.
 */
void doorController_openingDoor(void)
{
   if (doorStatus != opened) { // se a porta não estiver aberta, abrir.
      LCD_clearScreen();
      LCD_placeCursorInPosition(0x81);
      LCD_print("Status:", 10);

      LCD_placeCursorInPosition(0xC3);
      LCD_print("Abrindo...", 10);
      LCD_customDelay_ms(1500); // a porta gasta 1,5 s para abrir

      LCD_placeCursorInPosition(0xC3);
      LCD_print("Aberta    ", 10);
      
      doorStatus = opened;
   }
   CLRWDT();
}

/**
 * Procedimento para fechar a porta.
 */
void doorController_closingDoor(void)
{
   if (doorStatus != closed) { // se a porta não estiver fechada, fechar.
      LCD_clearScreen();
      LCD_placeCursorInPosition(0x81);
      LCD_print("Status:", 10);

      LCD_placeCursorInPosition(0xC3);
      LCD_print("Fechando...", 10);
      LCD_customDelay_ms(1500); // a porta gasta 1,5 s para fechar

      LCD_placeCursorInPosition(0xC3);
      LCD_print("Fechada    ", 10);
      
      doorStatus = closed;
   }
   CLRWDT();
}

/**
 * Procedimento para apresentar alerta de incêndio.
 */
void doorController_fire(void)
{
   LCD_clearScreen();
   LCD_placeCursorInPosition(0xC1);
   LCD_print("INCENDIO!", 10);

   LCD_placeCursorInPosition(0x91);
   LCD_print("Emergencia", 10);

   doorStatus = fire;
   
   CLRWDT();
}

/**
 * Procedimento para apresentar alerta de invasão.
 */
void doorController_invasion(void)
{
   LCD_clearScreen();
   LCD_placeCursorInPosition(0xC1);
   LCD_print("INVASAO!", 10);

   LCD_placeCursorInPosition(0x91);
   LCD_print("Emergencia", 10);

   doorStatus = invasion;
   
   CLRWDT();
}