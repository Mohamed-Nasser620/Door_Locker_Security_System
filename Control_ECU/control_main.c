/*
 * control_main.c
 *
 *  Created on: Oct 27, 2022
 *      Author: Mohamed
 */

#include <avr/io.h>
#include <util/delay.h>
#include "buzzer.h"
#include "external_eeprom.h"
#include "dc_motor.h"
#include "uart.h"
#include "i2c.h"
#include "timer1.h"
#include "common_macros.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define WRONG_BYTE            'w'  /* Byte defines wrong data sent to control_MCU */
#define CONFIRM_BYTE          'c'  /* Byte defines correct data sent to control_MCU */
#define REPEAT_BYTE           'r'  /* Byte defines wrong data sent to control_MCU and asks for repeating it */
#define EEPROM_ADDRESS         0   /* First Address in EEPROM Password */

/*******************************************************************************
 *                                    Globals                                  *
 *******************************************************************************/

/* defines if matching between passwords occurs or not */
uint8 g_matchingFlag = 0;

uint8 g_passArray [8] = {'.', '.', '.', '.', '.', '.', '.', '.'};         /* Array contains the new password */
uint8 g_repeatedPassArray [8] = {'.', '.', '.', '.', '.', '.', '.', '.'}; /* Array contains the confirm password */
uint8 g_definedPassArray [8] = {'.', '.', '.', '.', '.', '.', '.', '.'};  /* Array contains the user input system password */

/*Timer configuration for 15 sec */
TIMER1_ConfigType s_timerConfigurations_15Sec = {13885, 0, FCPU_1024, NORMAL};
/*Timer configuration for 3 sec */
TIMER1_ConfigType s_timerConfigurations_3Sec = {42099, 0, FCPU_1024, NORMAL};
/*Timer configuration for 1 min */
TIMER1_ConfigType s_timerConfigurations_60Sec = {55538, 0, FCPU_1024, NORMAL};

/*******************************************************************************
 *                             Functions Prototypes                            *
 *******************************************************************************/

/*
 * Description:
 * 1. Receive the new password and its confirmation from HMI_ECU.
 * 2. Compare the 2 passwords.
 * 3. If matched, send the confirm byte to HMI_ECU and save the password in EEPROM.
 * 4. If not matched, send the wrong byte to HMI_ECU and repeat again.
 */
void recieveCheckNewPassword (void);

/*
 * Description:
 * 1. Receive the user input password for selecting either open door or change pass from HMI_ECU.
 * 2. Compare it with the password saved in EEPROM.
 * 3. If matched, receive the user choice byte and if '+' rotate the motor, if '-' change password.
 * 4. If not matched, send repeat byte to HMI_ECU to ask for password 2 more times.
 * 5. If matched in the 2 next iterations take the action.
 * 6. If not matched in the 3 iterations, send wrong byte to HMI_ECU and start the buzzer.
 */
void getDefinedPassword (void);

/*
 * Description:
 * Timer1 first call back function after counting 15 seconds:
 * 1. First call stops the motor for 3 seconds after 15 seconds and initialize the timer for counting 3 seconds.
 * 2. Second call stops the motor after gate is closed and de-initialise the timer.
 */
void timerCallBack_15Sec (void);

/*
 * Description:
 * Timer1 second call back function after counting 3 seconds:
 * 1. After being called initialize the timer for counting another 15 seconds for the door to start locking.
 */
void timerCallBack_3Sec (void);

/*
 * Description:
 * Timer1 third call back function after counting 1 minute:
 * 1. After being called stops the buzzer ringing which started when 3 consecutive passwords are wrong.
 */
void timerCallBack_60Sec (void);


int main (void)
{
	BUZZER_init ();													/* Initialize buzzer */
	DcMotor_init ();												/* Initialize DC_motor */
	/* I2C configurations with address of 1 and 400 Kbit/sec (Fast Mode)*/
	TWI_ConfigType s_i2cConfiguration = {1, 400};
	TWI_init (&s_i2cConfiguration);
	/* UART configurations with 8 Bits data, No parity, one stop bit and 9600 baud rate*/
	UART_ConfigType s_uartConfiguration = {EIGHT_BITS, DISABLED, ONE_BIT, 9600};
	UART_init (&s_uartConfiguration);
	SET_BIT (SREG, 7);												/* Enable I-bit */

	for(;;)
	{
		/* When there is no matching between new and confirmation passwords receive new password again */
		if (g_matchingFlag == 0)
		{
			recieveCheckNewPassword ();
		}
		/* When the new pass and confirmation are matched start the system options by receiving the user choice*/
		else if (g_matchingFlag == 1)
		{
			getDefinedPassword ();
		}
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description:
 * Timer1 first call back function after counting 15 seconds:
 * 1. First call stops the motor for 3 seconds after 15 seconds and initialize the timer for counting 3 seconds.
 * 2. Second call stops the motor after gate is closed and de-initialise the timer.
 */
void timerCallBack_15Sec (void)
{
	static uint8 counter = 0;
	counter++;
	switch (counter)
	{
	case 2:

		DcMotor_stop ();						   /* Stop the motor after being unlocking for 15 seconds */
		TIMER1_deInit ();
		TIMER1_setCallBack (timerCallBack_3Sec);   /* Set the second call back */
		TIMER1_init (&s_timerConfigurations_3Sec); /* Start to count 3 seconds for door to start locking again */
		break;
	case 4:
		DcMotor_stop ();						   /* Stop the motor after being locked again */
		TIMER1_deInit ();						   /* De_initialize the timer */
		counter = 0;
	}
}

/*
 * Description:
 * Timer1 second call back function after counting 3 seconds:
 * 1. After being called initialize the timer for counting another 15 seconds for the door to start locking.
 */
void timerCallBack_3Sec (void)
{
	DcMotor_rotate (CCW, 100);					/* rotate motor CCW after being stopped for 3 seconds */
	TIMER1_deInit ();
	TIMER1_init (&s_timerConfigurations_15Sec); /* Start to count 15 seconds for door to be locked again */
	TIMER1_setCallBack (timerCallBack_15Sec);   /* Set the first call back */
}

/*
 * Description:
 * Timer1 third call back function after counting 1 minute:
 * 1. After being called stops the buzzer ringing which started when 3 consecutive passwords are wrong.
 */
void timerCallBack_60Sec (void)
{
	static uint8 counter = 0;
	counter++;
	/* Because it takes 8 ISR to count 1 minute */
	if (counter == 8)
	{
		BUZZER_off ();							/* Stop the buzzer after 1 minute */
		TIMER1_deInit ();						/* De_initialize the timer */
		counter = 0;
	}
}

/*
 * Description:
 * 1. Receive the new password and its confirmation from HMI_ECU.
 * 2. Compare the 2 passwords.
 * 3. If matched, send the confirm byte to HMI_ECU and save the password in EEPROM.
 * 4. If not matched, send the wrong byte to HMI_ECU and repeat again.
 */
void recieveCheckNewPassword (void)
{
	uint8 i = 0;
	uint8 breaking = 5;

	/* Receive the 2 passwords */
	UART_receiveString (g_passArray);
	UART_receiveString (g_repeatedPassArray);

	/* Compare the 2 passwords */
	while ((g_passArray[i] != '\0') && (g_repeatedPassArray[i] != '\0'))
	{
		if (g_passArray[i] != g_repeatedPassArray[i])
		{
			break;
		}
		i++;
		breaking--;
	}

	/* Success Case */
	if (breaking == 0)
	{
		UART_sendByte (CONFIRM_BYTE);                                         /* Send confirm byte */
		g_matchingFlag = 1;
		for (i = 0; i < 5; i++)
		{
			EEPROM_writeByte ((uint16)(EEPROM_ADDRESS + i), g_passArray[i]);  /* Save password in EEPROM */
			_delay_ms (10);
		}
	}
	/* Fail Case */
	else
	{
		UART_sendByte (WRONG_BYTE);											  /* Send wrong byte */
	}
}

/*
 * Description:
 * 1. Receive the user input password for selecting either open door or change pass from HMI_ECU.
 * 2. Compare it with the password saved in EEPROM.
 * 3. If matched, receive the user choice byte and if '+' rotate the motor, if '-' change password.
 * 4. If not matched, send repeat byte to HMI_ECU to ask for password 2 more times.
 * 5. If matched in the 2 next iterations take the action.
 * 6. If not matched in the 3 iterations, send wrong byte to HMI_ECU and start the buzzer.
 */
void getDefinedPassword (void)
{
	uint8 i = 0;
	uint8 breaking = 5;
	uint8 recieved = 0;
	static uint8 wrongIterations = 0;										  /* For counting the wrong pass */

	UART_receiveString (g_definedPassArray);								  /* Receive the user input pass */

	/* Receive the pass stored in EEPROM */
	for (i = 0; i < 5; i++)
	{
		EEPROM_readByte ((uint16)(EEPROM_ADDRESS + i), g_repeatedPassArray + i);
		_delay_ms (10);
	}
	g_repeatedPassArray[i] = '\0';

	/* Start comparing the 2 passwords */
	i = 0;
	while ((g_definedPassArray[i] != '\0') && (g_repeatedPassArray[i] != '\0'))
	{
		if (g_definedPassArray[i] != g_repeatedPassArray[i])
		{
			break;
		}
		i++;
		breaking--;
	}
	/* Success Case */
	if (breaking == 0)
	{
		UART_sendByte (CONFIRM_BYTE);                                         /* Send confirm byte */
		recieved = UART_recieveByte ();										  /* Receive the user choice */
		wrongIterations = 0;
		if (recieved == '+')												  /* If open the door */
		{
			DcMotor_rotate (CW, 100);										  /* Start rotating the motor CW */
			TIMER1_setCallBack (timerCallBack_15Sec);
			TIMER1_init (&s_timerConfigurations_15Sec);						  /* Rotate for 15 seconds */
		}
		else if (recieved == '-')											  /* If change pass */
		{
			g_matchingFlag = 0;												  /* For calling recieveCheckNewPassword */
		}
	}
	/* Fail Case */
	else
	{
		wrongIterations++;													  /* Increment wrong iterations */
		if (wrongIterations == 3)											  /* If it reaches 3 */
		{
			UART_sendByte (WRONG_BYTE);										  /* Send wrong byte */
			TIMER1_setCallBack (timerCallBack_60Sec);
			TIMER1_init (&s_timerConfigurations_60Sec);						  /* Start counting 60 seconds */
			BUZZER_on ();													  /* Start the buzzer */
			wrongIterations = 0;											  /* Restart the wrong iterations again */
		}
		else
		{
			UART_sendByte (REPEAT_BYTE);									  /* If less than 3 send repeat */
		}
	}
}
