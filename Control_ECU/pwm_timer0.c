/*
 * pwm_timer0.c
 *
 *  Created on: Oct 6, 2022
 *      Author: Mohamed
 */

#include <avr/io.h>
#include <math.h>
#include "common_macros.h"
#include "pwm_timer0.h"
#include "gpio.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/* Description :
 *1. Setup the PWM mode fot timer0 with Non-Inverting.
 *2. Setup the prescaler with F_CPU/8.
 *3. Setup the compare value based on the required input duty cycle
 *4. Setup the direction for OC0 as output pin through the GPIO driver.
 *5. The generated PWM signal frequency will be 500Hz to control the DC Motor speed.
 */
void PWM_Timer0_start(uint8 duty_cycle)
{
	TCNT0 = 0;                       /* Set timer register initial value to 0 */
	/* Fast PWM Mode WGM01 = 1 & WGM00 = 1 */
	SET_BIT (TCCR0, WGM00);
	SET_BIT (TCCR0, WGM01);

	SET_BIT (TCCR0, COM01);          /* Clear OC0 when match occurs (non inverted mode) COM00 = 0 & COM01 = 1 */
	/* Clock = F_CPU/64 by making CS00 = 1, CS01 = 1, CS02 = 0 */
	SET_BIT (TCCR0, CS00);
	SET_BIT (TCCR0, CS01);
	OCR0 = ceil ((duty_cycle / 100.0) * TIMER0_TOP_VALUE);    /* Set compare value */
	GPIO_setupPinDirection (PORTB_ID, PIN3_ID, PIN_OUTPUT);   /* Configure PB3/OC0 as output pin */
}
