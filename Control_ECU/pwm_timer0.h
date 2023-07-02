/*
 * pwm_timer0.h
 *
 *  Created on: Oct 6, 2022
 *      Author: Mohamed
 */

#ifndef PWM_TIMER0_H_
#define PWM_TIMER0_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/* Parameters Definitions */
#define TIMER0_TOP_VALUE            255

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/* Description :
 *1. Setup the PWM mode fot timer0 with Non-Inverting.
 *2. Setup the prescaler with F_CPU/8.
 *3. Setup the compare value based on the required input duty cycle
 *4. Setup the direction for OC0 as output pin through the GPIO driver.
 *5. The generated PWM signal frequency will be 500Hz to control the DC Motor speed.
 */
void PWM_Timer0_start(uint8 duty_cycle);

#endif /* PWM_TIMER0_H_ */
