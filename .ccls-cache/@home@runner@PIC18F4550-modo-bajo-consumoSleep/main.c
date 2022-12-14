/*
 * File:   Sistema Bajo consumo PIC18F4550.c
 * Author: mario
 *
 * Created on 5 de Agosto de 2022, 08:42 AM
 */

#include <xc.h>
#define _XTAL_FREQ 48000000      //Frecuencia de trabajo
#pragma config PLLDIV = 5
#pragma config CPUDIV = OSC1_PLL2
#pragma config FOSC = HSPLL_HS  //Configuración de fuente de reloj interna
#pragma config WDT=OFF          //Desactivación del perro guardian
#pragma LVP=OFF                 //Programación de Bajo voltaje Activada

#define LED_CPU LATC2           //Pin del led de CPU asociado al funcionamiento

void __interrupt () ISR (void); //Interrupción


void main(void) {
  OSCCON=0b11110000;//Configuración del oscilador 
  __delay_ms(1);    //Tiempo de establecimiento para el oscilador
  /* Configuración del puerto C */
  LATC=0;           //Inicializar el puerto en 0
  ADCON1=0x0F;      /* Configurar como digital todos los pines analodos
                     *En caso de requerir usar algun puerto analogo modificar
                     * este registro (Pag 266 datasheet)
                     */
  TRISC  = 0;       //Configuración del puerto E como Salida
  /* Configuración de timer 0 */
  T0CON=0b00000111; /*Configuración del timer0 correspondiente 
                     * a un tiempo aproximado de 0.5seg usando la ecuación
                     * (2^N)-((Tsflujo*Fbus)/PS)= Precarga
                     * La precarga debe de ser menor a 2^N que es la
                     * resolución del timer 
                     * 6^16 -((0.5seg*2x10^6Hz)/16)=3036
                     * La precarga se suma al valor del registro TMR0L
                     */
  TMR0ON=1;         //Encendido del Timer 0
  /*    Configuración de las interrupciones    */
  INTEDG1=1;
  INT1IE=1;
  INT1IF=0;
  
  TMR0IF=0; //Bandera de Interrupción timer 0 en Cero
  TMR0IE=1; //Activación de la Interrupción para desbordamiento Timer0
  PEIE=1;   //Habilitaciónon de las interrupciones por periferico
  GIE=1;    //Habilitación de las interrupciones generales
  
  
  TRISB=0xFF;
  while(1){
      
      if(RB2==0){
          
        SCS1=1;
        SCS0=0;
          
      }
      else{
        SCS1=0;
        SCS0=0;
      }
      if(RB0==1){
          LED_CPU=0;
          TMR0ON=0;
          SLEEP();
          TMR0ON=1;
          TMR0L=0;
          TMR0H=0;
      }
      
  }
}
/* Función de interrupción */
void __interrupt () ISR (void){ //Interrupción
    if(TMR0IF==1){
        LED_CPU=LED_CPU ^ 1;
        /*TMR0L=3036 & 255; /* para cargar el valor de precarga se divide
                           el numero en dos registros el alto y el bajo
                           para la parte alta baja se toma la precarga y
                           se realiza una and de 255 (0b1111 1111) para 
                           que de esta manera solo se tenga el Valor bajo*/
        /*TMR0H=3036 >> 8;  /* Para la parte alte se realiza un corrimiento de 
                           el valor de precarga de ocho posiciones*/
        TMR0IF=0;         /*se pone la bandera de interrupción en 0 para que
                           vuelva a ocurrir*/
        LATC0=0;
      }
    if(INT1F==1){
        LATC0=1;
        INT1F=0;
    }
}
