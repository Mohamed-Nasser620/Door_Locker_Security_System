/******************************************************************************
 *
 * Module: DC_MOTOR
 *
 * File Name: dc_motor.c
 *
 * Author: Mohamed Nasser
 *
 * Date Created: Oct 5, 2022
 *
 * Description: Source file for the DC_motor module driver
 *
 *******************************************************************************/

#include "DC_motor.h"
#include "gpio.h"
#include "pwm_timer0.h"

/*******************************************************************************
 *                          Functions Definitions                              *
 *******************************************************************************/

/*
 * Description :
 * 1. The Function responsible for setup the direction for the two motor pins.
 * 2. Stop the DC-Motor at the beginning.
 */
void DcMotor_init (void)
{
	/* Set the two motor pins as output pins */
	GPIO_setupPinDirection (DC_PORT, DC_IN1_PIN, PIN_OUTPUT);
	GPIO_setupPinDirection (DC_PORT, DC_IN2_PIN, PIN_OUTPUT);

	/* Stop the motor at the beginning */
	GPIO_writePin (DC_PORT, DC_IN1_PIN, LOGIC_LOW);
	GPIO_writePin (DC_PORT, DC_IN2_PIN, LOGIC_LOW);
}

/*
 * Description :
 * 1. The function responsible for rotate the DC Motor CW/ or CCW or
 * stop the motor based on the direction input value.
 * 2. Send the required duty cycle to the PWM driver based on the required speed value.
 */
void DcMotor_rotate (DcMotor_Direction dir, uint8 speed)
{
	uint8 dutyCycle = 0;

	/* Set the out put of the two motor pins to change its rotation direction depending on the input */
	if (dir == CW)
	{
		GPIO_writePin (DC_PORT, DC_IN1_PIN, LOGIC_LOW);
		GPIO_writePin (DC_PORT, DC_IN2_PIN, LOGIC_HIGH);
	}

	else if (dir == CCW)
	{
		GPIO_writePin (DC_PORT, DC_IN1_PIN, LOGIC_HIGH);
		GPIO_writePin (DC_PORT, DC_IN2_PIN, LOGIC_LOW);
	}

	/* The equation to transform the speed into duty cycle and send to the timer driver */
	dutyCycle = (uint8)(((float32)speed / DC_MAX_SPEED) * 100);
	PWM_Timer0_start (dutyCycle);
}

/*
 * Description :
 * 1. The Function responsible for stop the motor rotation by stoping the two motor pins.
 * 2. Stop PWM wave generation.
 */
void DcMotor_stop (void)
{
	DcMotor_rotate (CW, DC_MIN_SPEED);                    /* Stop the PWM wave generation */
	GPIO_writePin (DC_PORT, DC_IN1_PIN, LOGIC_LOW);       /* Stop the first motor pin */
	GPIO_writePin (DC_PORT, DC_IN2_PIN, LOGIC_LOW);       /* Stop the second motor pin */
}
