/************************************************************
 Biblioteca para LCD utilizando a placa PICGenios - PICSimLab
 Compilador XC8
 IDE: MPLAB X
 ***********************************************************/

#ifndef LCD_H
#define LCD_H

#include <string.h>
#include <math.h>

void LCD_init()
{
   TRISD = 0b00000000; //Configura a Porta D como saída - porta de instruções e dados
   TRISE = 0b00000000; //Configura a Porta E como saída - porta de controle

   PORTEbits.RE2 = 0; //RS = 0 - modo instrução
   int i;
   char config[7] = {//Conjunto de comandos para configurar o LCD
      0x30, //3 comandos para estabilizar o LCD
      0x30,
      0x30,
      0x38, //Comunicacao de 8bits. Display de duas ou mais linhas. Matriz de 8x5.
      0x0F, //Liga o cursor com alternancia.
      0x06, //Desloca o cursor para a direita quando um caractere e? inserido, mas nao desloca a mensagem.
      0x01 //Limpa a tela do display e desloca o cursor para a linha 1 e coluna 1. Apaga o conteu?do da DDRAM.
   };

   for (i = 0; i < 7; i++) {
      PORTEbits.RE1 = 1; //Habilita o LCD para receber comando ou dado
      PORTD = config[i]; //Envia o comando
      __delay_us(50);
      PORTEbits.RE1 = 0; //Desabilita o LCD para receber comando ou dado
      __delay_us(50);
   }
   PORTEbits.RE2 = 1; //Finaliza a recepcao de comandos do LCD
}

void LCD_clearScreen()
{
   PORTEbits.RE2 = 0;
   PORTEbits.RE1 = 1;
   PORTD = 0x01;
   __delay_us(50);
   PORTEbits.RE1 = 0;
   __delay_us(50);
   PORTEbits.RE2 = 1;
}

void LCD_placeCursorInPosition(char position)
{
   PORTEbits.RE2 = 0; //Inicia a recepcao de comandos do LCD
   PORTEbits.RE1 = 1;
   PORTD = position;
   __delay_us(50);
   PORTEbits.RE1 = 0;
   __delay_us(50);
   PORTEbits.RE2 = 1; //Finaliza a recepcao de comandos do LCD
}

void LCD_customDelay_ms(long ms)
{
   while (ms--) {
      __delay_ms(1);
   }
}

void LCD_print(char text[], long delay)
{
   int i, length;
   length = (int) strlen(text); //Tamanho da string a ser escrita
   for (i = 0; i < length; i++) {
      PORTEbits.RE1 = 1;
      PORTD = text[i]; //Envia um caracter por ver para o LCD
      LCD_customDelay_ms(delay);
      //__delay_ms(50); //Se quiser uma escrita imperceptivel, reduzir essa espera até o mínimo de 50us.
      PORTEbits.RE1 = 0;
      __delay_us(50);
   }
}

void LCD_printChar(char letter)
{
   PORTEbits.RE1 = 1;
   PORTD = letter; //Envia um caracter por ver para o LCD
   __delay_us(50); //Se quiser uma escrita imperceptivel, reduzir essa espera até o mínimo de 50us.
   PORTEbits.RE1 = 0;
   __delay_us(50);
}

void LCD_printFloatNumber(double number, int precision)
{
   char digit, counter, aux;
   counter = 0;
   aux = 0;
   while (number >= 1) {
      number /= 10;
      counter++;
   }
   if (counter == 0) {
      LCD_printChar(48);
      LCD_printChar(46);
   }
   while (/*numero>0 &&*/ aux < counter + precision) {
      aux++;
      number *= 10;
      digit = floor(number);
      number -= digit;
      LCD_printChar(48 + digit);
      if (aux == counter)
         LCD_printChar(46); //ponto para o decimal
   }
}


#endif