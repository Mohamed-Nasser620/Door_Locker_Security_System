/******************************************************************************
 *
 * Module: Timer 1
 *
 * File Name: timer1.h
 *
 * Author: Mohamed Nasser
 *
 * Date Created: Oct 27, 2022
 *
 * Description: Header file for the timer 1 AVR driver
 *
 *******************************************************************************/

#ifndef TIMER1_H_
#define TIMER1_H_

#include "std_types.h"

/*******************************************************************************
 *                               Enumerations                                  *
 *******************************************************************************/
typedef enum
{
	OFF, FCPU_1, FCPU_8, FCPU_64, FCPU_256, FCPU_1024
}TIMER1_Prescaler;

typedef enum
{
	NORMAL, CTC = 4
}TIMER1_Mode;

/*******************************************************************************
 *                     Structures And Unions                                   *
 *******************************************************************************/
typedef struct {
uint16 initial_value;
uint16 compare_value; // it will be used in compare mode only.
TIMER1_Prescaler prescaler;
TIMER1_Mode mode;
} TIMER1_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description:
 * Function to initialize the Timer1 driver.
 * 1. Set the timer initial value.
 * 2. Set the compare value if needed.
 * 3. Select the required pre-scaler.
 * 4. Select the timer mode.
 */
void TIMER1_init(const TIMER1_ConfigType * Config_Ptr);

/*
 * Description:
 * Function to set the Call Back function address.
 */
void TIMER1_setCallBack(void(*a_ptr)(void));

/*
 * Description:
 * Function to disable the Timer1.
 */
void TIMER1_deInit(void);

#endif /* TIMER1_H_ */
