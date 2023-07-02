/******************************************************************************
 *
 * Module: DC_MOTOR
 *
 * File Name: dc_motor.h
 *
 * Author: Mohamed Nasser
 *
 * Date Created: Oct 5, 2022
 *
 * Description: Header file for the DC_motor module driver
 *
 *******************************************************************************/

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/* Static Configurations */
#define DC_PORT           PORTB_ID
#define DC_IN1_PIN        PIN6_ID
#define DC_IN2_PIN        PIN7_ID

/* Parameters Definitions */
#define DC_MAX_SPEED      100
#define DC_MIN_SPEED      0
#define DC_FREQUENCY      500

/*******************************************************************************
 *                               Enumerations                                  *
 *******************************************************************************/
typedef enum
{
	CW, CCW
} DcMotor_Direction;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * 1. The Function responsible for setup the direction for the two motor pins.
 * 2. Stop the DC-Motor at the beginning.
 */
void DcMotor_init (void);

/*
 * Description :
 * 1. The function responsible for rotate the DC Motor CW/ or CCW or
 * stop the motor based on the direction input value.
 * 2. Send the required duty cycle to the PWM driver based on the required speed value.
 */
void DcMotor_rotate (DcMotor_Direction dir, uint8 speed);

/*
 * Description :
 * 1. The Function responsible for stop the motor rotation by stoping the two motor pins.
 * 2. Stop PWM wave generation.
 */
void DcMotor_stop (void);

#endif /* DC_MOTOR_H_ */
