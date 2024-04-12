/************************************************************************************************
Company:
Microside Technology Inc.
File Name:
Servo.c
Product Revision  :  1
Driver Version    :  0.9beta1

Disclaimer:
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** OF MERCHANTABILITY, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
** TO THE WARRANTIES FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM,
** DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
** OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
** OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
************************************************************************************************/

/*!
 * @file  Servo.h
 * @brief Small Servo library using Timer1 interruption technique.
          For testing purposes only DO NOT USE on producion.
 */
 
// TODO: This library throws a warning for a possible race condition, fix by 
//       initial release

#ifndef _SERVO_H_
#define _SERVO_H_

// -------------------------------------------------------------- PUBLIC MACROS

// If needed to be used with a different FOSC, define this value to the
// appropiate frequency in Hertz.
#ifndef SERVO_FOSC
#define SERVO_FOSC 48000000
#endif

// Hobby servos work by measuring the duty cycle from 1ms to 2ms, then adjusting
// the position proportionately from 0 to 180, however most of them have different
// offsets, calibrate the following values to match your servo position 

// Seconds that generate a 0° turn
#ifndef SERVO_MIN_POS
#define SERVO_MIN_POS 0.0004
#endif

// Seconds that generate a 180° turn
#ifndef SERVO_MAX_POS
#define SERVO_MAX_POS 0.0027
#endif

// Interval between 0° and 180° turn in seconds
#define SERVO_DUTY_CYCLE_POS (SERVO_MAX_POS - SERVO_MIN_POS)

// Time needed to complement a 50Hz
#define SERVO_FREQ_COMPLEMENT 0.02 - SERVO_MAX_POS

// Timer1 preescaler used, bigger values reduce control accuracy, values too
// small might result in errors.
#ifndef SERVO_T_PREESCALER
#define SERVO_T_PREESCALER 2
#endif

// Values for Timer1 interruption 
#define SERVO_MIN_POS_T1_VALUE (unsigned int16)(65535 - ( (SERVO_FOSC * SERVO_MIN_POS)/(4 * SERVO_T_PREESCALER) ))

#define SERVO_FREQ_COMPLEMENT_VALUE (unsigned int16)(65535 - ( (SERVO_FOSC * SERVO_FREQ_COMPLEMENT)/(4 * SERVO_T_PREESCALER) ))

#define SERVO_MAX_COMPLEMENT (unsigned int16)(65535 - ( ( SERVO_FOSC * ( SERVO_DUTY_CYCLE_POS ) ) / (4 * SERVO_T_PREESCALER) ) )

// ------------------------------------------------------------------ VARIABLES

// Counter used to keep track of the pin level and the preloaded value for Timer1
unsigned int8 __servo_array_pointer = 0;

// Signal generation array and pin status
// TODO: implement multiple server control, it should be doable to control up to 16 servos
// using only Timer1, rewriting this section might also reduce memory consumtion
unsigned int16 __servoSignal[4] = { SERVO_MIN_POS_T1_VALUE, 65535, SERVO_MAX_COMPLEMENT, SERVO_FREQ_COMPLEMENT_VALUE };
unsigned int8  __onOffMap[4] = {1,1,0,0};

// Output pin
int16 __servo_pin = -1;

// ----------------------------------------------- PUBLIC FUNCTION DECLARATIONS

/**
 * @brief Interrupt handler.
 * 
 * @description This function preload Timer1 next value and changes the output
 *              pin
 */
#int_timer1
void TIMER1_isr() {
   __servo_array_pointer++;
   if(__servo_array_pointer > 3) __servo_array_pointer = 0;
   setup_timer_1 ( T1_DISABLED );
   set_timer1( __servoSignal[__servo_array_pointer] );
   if ( __onOffMap[__servo_array_pointer] == 1 )
      output_high(__servo_pin);
    else
      output_low(__servo_pin);
    setup_timer_1 ( T1_INTERNAL | T1_DIV_BY_2 );
}

/**
 * @brief Servo signal setup.
 * 
 * @param Output pin
 *
 * @description This function sets Timer1, Global interruptions and the output
 *              pin
 */
void setup_servo( int16 output_pin ) {
   __servo_array_pointer = 0;
   
   set_timer1( 0 );
   
   enable_interrupts( INT_TIMER1 );
   enable_interrupts( GLOBAL );
   
   __servo_pin = output_pin;
   
   output_HIGH (__servo_pin);
   
   setup_timer_1 ( T1_INTERNAL | T1_DIV_BY_2 );
}

/**
 * @brief Sets control signal in seconds.
 * 
 * @param Seconds
 *
 * @description This function calculates Timer1 overflow value for the given
 *              seconds, then loads this value on Timer1 overflow registers
 */
void set_servo_ms( float seconds ) {
   if( seconds > SERVO_DUTY_CYCLE_POS )
      seconds = SERVO_DUTY_CYCLE_POS;
   if( seconds < 0 )
      seconds = 0;
   unsigned int16 starting_point = (unsigned int16)(65535 - ( (SERVO_FOSC * seconds)/(4 * SERVO_T_PREESCALER) ));
   unsigned int16 complement = (unsigned int16)(65535 - ( (SERVO_FOSC * (SERVO_DUTY_CYCLE_POS - seconds))/(4 * SERVO_T_PREESCALER) ));
   __servoSignal[1] = starting_point;
   __servoSignal[2] = complement;
}

/**
 * @brief Sets the servo angle.
 * 
 * @param angle in degrees
 *
 * @description This function calculates the time in seconds that turn the
 *              servo to the input angle
 */
void set_servo_angle ( int angle ) {
   if( angle > 180 )
      angle = 180;
   float __t = (SERVO_DUTY_CYCLE_POS/180.0) * (float)angle;
   set_servo_ms( __t );
}

#endif // _SERVO_H_
