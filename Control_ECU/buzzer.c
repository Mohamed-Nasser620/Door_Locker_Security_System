/******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.c
 *
 * Author: Mohamed Nasser
 *
 * Date Created: Oct 27, 2022
 *
 * Description: Source file for the Buzzer Module driver
 *
 *******************************************************************************/

#include "buzzer.h"
#include "gpio.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description:
 * 1. Setup the direction for the buzzer pin as output pin through the GPIO driver.
 * 2. Turn off the buzzer through the GPIO.
 */
void BUZZER_init(void)
{
	GPIO_setupPinDirection (BUZZER_PORT, BUZZER_PIN, PIN_OUTPUT);
	GPIO_writePin (BUZZER_PORT, BUZZER_PIN, BUZZER_OFF);
}

/*
 * Description:
 * Function to enable the Buzzer through the GPIO.
 */
void BUZZER_on(void)
{
	GPIO_writePin (BUZZER_PORT, BUZZER_PIN, BUZZER_ON);
}

/*
 * Description:
 * Function to disable the Buzzer through the GPIO.
 */
void BUZZER_off(void)
{
	GPIO_writePin (BUZZER_PORT, BUZZER_PIN, BUZZER_OFF);
}
