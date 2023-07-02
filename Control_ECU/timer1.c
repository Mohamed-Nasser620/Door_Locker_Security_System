/******************************************************************************
 *
 * Module: Timer 1
 *
 * File Name: timer1.c
 *
 * Author: Mohamed Nasser
 *
 * Date Created: Oct 27, 2022
 *
 * Description: Source file for the timer 1 AVR driver
 *
 *******************************************************************************/

#include "timer1.h"
#include "std_types.h"
#include "common_macros.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/*******************************************************************************
 *                                    Globals                                  *
 *******************************************************************************/
static volatile void (*g_callBack_Ptr)(void) = NULL_PTR;

/*******************************************************************************
 *                                    ISR                                      *
 *******************************************************************************/

/* For calling the call back functions */
ISR (TIMER1_COMPA_vect)
{
	if (g_callBack_Ptr != NULL_PTR)
		{
			(*g_callBack_Ptr)();
		}
}

ISR (TIMER1_OVF_vect)
{
	if (g_callBack_Ptr != NULL_PTR)
	{
		(*g_callBack_Ptr)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description:
 * Function to initialize the Timer1 driver.
 * 1. Set the timer initial value.
 * 2. Set the compare value if needed.
 * 3. Select the required pre-scaler.
 * 4. Select the timer mode.
 */
void TIMER1_init(const TIMER1_ConfigType * Config_Ptr)
{
	TCCR1A = 0x0C;       											/* For selecting non_PWM mode */
	TCCR1B = ((Config_Ptr -> mode) & 0x0F) << 4;        			/* For selecting the mode */
	TCCR1B = (TCCR1B & 0xF8) | ((Config_Ptr -> prescaler) & 0x07);	/* For selecting the pre-scaler */
	TCNT1 = Config_Ptr -> initial_value;							/* Set the initial timer value */
	OCR1A = Config_Ptr -> compare_value;							/* Set the required compare value */
	/* Enable the Interrupts for the module */
	switch (Config_Ptr -> mode)
	{
	case NORMAL:
		SET_BIT(TIMSK, TOIE1);
		break;
	case CTC:
		SET_BIT(TIMSK, OCIE1A);
	}
}

/*
 * Description:
 * Function to set the Call Back function address.
 */
void TIMER1_setCallBack(void(*a_ptr)(void))
{
	g_callBack_Ptr = (volatile void*)a_ptr;
}

/*
 * Description:
 * Function to disable the Timer1.
 */
void TIMER1_deInit(void)
{
	TCCR1B = (TCCR1B & 0xF8) | (0);                                 /* Stop the clock source */
}
