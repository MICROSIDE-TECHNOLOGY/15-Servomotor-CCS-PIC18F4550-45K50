/************************************************************************************************
Company:
Microside Technology Inc.
File Name:
Servomotor.c
Product Revision  :  1
Device            :  X-TRAINER
Driver Version    :  1.0
************************************************************************************************/

/*
---------------------------------------------------------------------------
Controlar la posic�on de un servo por medio de una se�al PWM
---------------------------------------------------------------------------
*/

#include <18F4550.h>                            //Incluye el microcontrolador con el que se va a trabajar
#include "Servo.h"                       // Librer�a para controlar 1 servo (Utiliza Timer1)
#use delay(clock=48Mhz, crystal)                //Tipo de oscilador y frecuencia dependiendo del microcontrolador 
#build(reset=0x02000,interrupt=0x02008)         //Asignaci�n de los vectores de reset e interrupci�n
#org 0x0000,0x1FFF {}                           //Reserva espacio en la memoria para la versi�n con bootloader

void main( void ) {
   set_tris_c( 0xFB );                      // Configura C2 como salida
   setup_servo( PIN_C2 );                   // Configura el PIN C2 como el servo

   while ( 1 ) {
      delay_ms( 1000 );
      set_servo_angle (0);                  // Configura la posici�n del servo
      delay_ms( 1000 );
      set_servo_angle (45);
      delay_ms( 1000 );
      set_servo_angle (90);
      delay_ms( 1000 );
      set_servo_angle (135);
      delay_ms( 1000 );
      set_servo_angle (180);
   }
}
