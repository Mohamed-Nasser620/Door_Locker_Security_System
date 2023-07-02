/******************************************************************************
 *
 * Module: BUZZER
 *
 * File Name: buzzer.h
 *
 * Author: Mohamed Nasser
 *
 * Date Created: Oct 27, 2022
 *
 * Description: Header file for the Buzzer Module driver
 *
 *******************************************************************************/

#ifndef BUZZER_H_
#define BUZZER_H_

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/* Connection Mode */
#define BUZZER_POSITIVE_LOGIC

#ifdef BUZZER_POSITIVE_LOGIC
#define BUZZER_ON                 LOGIC_HIGH
#define BUZZER_OFF                LOGIC_LOW
#endif

#ifdef BUZZER_NEGATIVE_LOGIC
#define BUZZER_ON                 LOGIC_LOW
#define BUZZER_OFF                LOGIC_HIGH
#endif

/* Static Configurations */
#define BUZZER_PORT           	  PORTD_ID
#define BUZZER_PIN      		  PIN6_ID

/* Parameters Definitions */
#define BUZZER_MAX_VOLTAGE    	  5

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description:
 * 1. Setup the direction for the buzzer pin as output pin through the GPIO driver.
 * 2. Turn off the buzzer through the GPIO.
 */
void BUZZER_init(void);

/*
 * Description:
 * Function to enable the Buzzer through the GPIO.
 */
void BUZZER_on(void);

/*
 * Description:
 * Function to disable the Buzzer through the GPIO.
 */
void BUZZER_off(void);

#endif /* BUZZER_H_ */
