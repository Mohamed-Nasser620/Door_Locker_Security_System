/*
 * hmi_main.c
 *
 *  Created on: Oct 29, 2022
 *      Author: Mohamed Nasser
 */

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timer1.h"
#include "common_macros.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define WRONG_BYTE            'w'  /* Byte defines wrong data sent to control_MCU */
#define CONFIRM_BYTE          'c'  /* Byte defines correct data sent to control_MCU */
#define REPEAT_BYTE           'r'  /* Byte defines wrong data sent to control_MCU and asks for repeating it */

/*******************************************************************************
 *                                    Globals                                  *
 *******************************************************************************/

/* defines if matching between passwords occurs or not */
uint8 g_matchingFlag = WRONG_BYTE;

uint8 g_passArray [8] = {'.', '.', '.', '.', '.', '.', '.', '.'};         /* Array contains the new password */
uint8 g_repeatedPassArray [8] = {'.', '.', '.', '.', '.', '.', '.', '.'}; /* Array contains the confirm password */
uint8 g_definedPassArray [8] = {'.', '.', '.', '.', '.', '.', '.', '.'};  /* Array contains the user input system password */

TIMER1_ConfigType s_timerConfigurations_15Sec = {0, 14649, FCPU_1024, CTC}; /*Timer configuration for 15 sec */
TIMER1_ConfigType s_timerConfigurations_3Sec = {0, 2930, FCPU_1024, CTC};   /*Timer configuration for 3 sec */
TIMER1_ConfigType s_timerConfigurations_60Sec = {0, 58594, FCPU_1024, CTC}; /*Timer configuration for 1 min */

/*******************************************************************************
 *                             Functions Prototypes                            *
 *******************************************************************************/

/*
 * Description:
 * 1. Tell the user to enter new password and saves it into array.
 * 2. Tell the user to confirm this new password and saves it into another array.
 * 3. Put '#' and '\0' in the end of the 2 arrays for sending them via UART.
 * 4. Send the 2 strings with UART to control_ECU.
 * 5. Wait for the confirmation or rejection from control_ECU to decide the next step.
 */
void takeNewPassword (void);

/*
 * Description:
 * 1. Print the main system options.
 * 2. Take the user choice if open the gate or change the password and send it to control_ECU.
 * 3. Ask for the password to confirm it is the real user and send it to control_ECU.
 * 4. If the password is correct, take action asked by the user.
 * 5. If the password is wrong, ask for it 2 more times.
 * 6. If wrong for the third time, display the warning message.
 */
void mainSystemDisplay (void);

/*
 * Description:
 * 1. Take the confirmation password for taking the user's action and send it to control_ECU.
 */
void repeatPassword (void);

/*
 * Description:
 * Timer1 first call back function after counting 15 seconds:
 * 1. First call tells that gate is opened now after 15 seconds and initialize the timer for counting 3 seconds.
 * 2. Second call return to display the system main options after gate is closed and de-initialise the timer.
 */
void timerCallBack_15Sec (void);

/*
 * Description:
 * Timer1 second call back function after counting 3 seconds:
 * 1. After being called initialize the timer for counting another 15 seconds for displaying door is locking.
 */
void timerCallBack_3Sec (void);

/*
 * Description:
 * Timer1 third call back function after counting 1 minute:
 * 1. After being called stops the displaying of warning message appears when 3 consecutive passwords are wrong.
 */
void timerCallBack_60Sec (void);


int main (void)
{
	LCD_init ();                                                                 /* Initialize LCD */
	/* UART configurations with 8 Bits data, No parity, one stop bit and 9600 baud rate*/
	UART_ConfigType s_configuration = {EIGHT_BITS, DISABLED, ONE_BIT, 9600};
	UART_init (&s_configuration);
	SET_BIT (SREG, 7);                                                           /* Enable I-bit */

	for(;;)
	{
		/* When there is no matching between new and confirmation passwords take new password again */
		if (g_matchingFlag == WRONG_BYTE)
		{
			takeNewPassword ();
		}
		/* When the new pass and confirmation are matched start the system options */
		else if (g_matchingFlag == CONFIRM_BYTE)
		{
			mainSystemDisplay ();
		}
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description:
 * Timer1 first call back function after counting 15 seconds:
 * 1. First call tells that gate is opened now after 15 seconds and initialize the timer for counting 3 seconds.
 * 2. Second call return to display the system main options after gate is closed and de-initialise the timer.
 */
void timerCallBack_15Sec (void)
{
	switch (g_matchingFlag)
	{
	case 'd':
		/* Display door is unlocked after being unlocking for 15 seconds */
		LCD_clearScreen ();
		LCD_moveCursor (0,5);
		LCD_displayString ("DOOR IS");
		LCD_moveCursor (1,4);
		LCD_displayString ("UNLOCKED");

		TIMER1_init (&s_timerConfigurations_3Sec); /* Start to count 3 seconds for door to start locking again */
		TIMER1_setCallBack (timerCallBack_3Sec);   /* Set the second call back */
		g_matchingFlag = 'e';
		break;
	case 'e':
		g_matchingFlag = CONFIRM_BYTE;             /* For system main options */
		TIMER1_deInit ();						   /* De_initialize the timer */
	}
}

/*
 * Description:
 * Timer1 second call back function after counting 3 seconds:
 * 1. After being called initialize the timer for counting another 15 seconds for displaying door is locking.
 */
void timerCallBack_3Sec (void)
{
	/* Display door is locking after being unlocked for 3 seconds */
	LCD_clearScreen ();
	LCD_moveCursor (0,4);
	LCD_displayString ("DOOR IS");
	LCD_moveCursor (1,4);
	LCD_displayString ("LOCKING");

	TIMER1_init (&s_timerConfigurations_15Sec);  /* Start to count 15 seconds for door to be locked again */
	TIMER1_setCallBack (timerCallBack_15Sec);	 /* Set the first call back */
}

/*
 * Description:
 * Timer1 third call back function after counting 1 minute:
 * 1. After being called stops the displaying of warning message appears when 3 consecutive passwords are wrong.
 */
void timerCallBack_60Sec (void)
{
	TIMER1_deInit ();                    /* De_initialize the timer */
	g_matchingFlag = CONFIRM_BYTE;       /* For system main options */
}

/*
 * Description:
 * 1. Tell the user to enter new password and saves it into array.
 * 2. Tell the user to confirm this new password and saves it into another array.
 * 3. Put '#' and '\0' in the end of the 2 arrays for sending them via UART.
 * 4. Send the 2 strings with UART to control_ECU.
 * 5. Wait for the confirmation or rejection from control_ECU to decide the next step.
 */
void takeNewPassword (void)
{
	uint8 i = 0;

	/* The Password */
	LCD_clearScreen();
	LCD_displayString ("PLZ ENTER PASS:");
	LCD_moveCursor (1,0);

	for (i = 0; i <= 5; i++)
	{
		g_passArray [i] = KEYPAD_getPressedKey ();
		if (g_passArray [i] == 13)
		{
			break;
		}
		LCD_sendData ('*');
		_delay_ms (450);
	}
	g_passArray[i] = '#';                              /* For UART_recieveString function */
	g_passArray[i+1] = '\0';						   /* For UART_sendString function */

	/* The repeated password */
	LCD_clearScreen();
	LCD_displayString ("PLZ RE-ENTER THE");
	LCD_moveCursor (1,0);
	LCD_displayString ("SAME PASS: ");
	LCD_moveCursor (1,10);

	for (i = 0; i <= 5; i++)
	{
		g_repeatedPassArray [i] = KEYPAD_getPressedKey ();
		if (g_repeatedPassArray [i] == 13)
		{
			break;
		}
		LCD_sendData ('*');
		_delay_ms (450);
	}
	g_repeatedPassArray[i] = '#';					  /* For UART_recieveString function */
	g_repeatedPassArray[i+1] = '\0';				  /* For UART_sendString function */

	/* Send the 2 strings to control_ECU and wait for confirmation */
	UART_sendString (g_passArray);
	UART_sendString (g_repeatedPassArray);
	g_matchingFlag = UART_recieveByte ();
}

/*
 * Description:
 * 1. Print the main system options.
 * 2. Take the user choice if open the gate or change the password and send it to control_ECU.
 * 3. Ask for the password to confirm it is the real user and send it to control_ECU.
 * 4. If the password is correct, take action asked by the user.
 * 5. If the password is wrong, ask for it 2 more times.
 * 6. If wrong for the third time, display the warning message.
 */
void mainSystemDisplay (void)
{
	static uint8 userChoice = 0;                    /* Save the characters '+' or '-' */
	static uint8 recieved = 0;						/* Save the received confirmation byte from control_ECU */

	/* Print the main system options for first time and disable it for second and third password iterations */
	if (recieved != REPEAT_BYTE)
	{
		LCD_clearScreen ();
		LCD_moveCursor (0,0);
		LCD_displayString ("+ : OPEN DOOR");
		LCD_moveCursor (1,0);
		LCD_displayString ("- : CHANGE PASS");

		userChoice = KEYPAD_getPressedKey ();       /* Take the user choice either open door or change pass */
		_delay_ms (300);
	}

	/*
	 * Depending on the choice take the confirmation pass and send it to control_ECU
	 * then receive the confirmation.
	 */
	switch (userChoice)
	{
	case '+':
		repeatPassword ();
		recieved = UART_recieveByte ();
		/* Depending on the received byte:
		 * 1. If confirm, open the door.
		 * 2. If wrong after 3 iterations, open the buzzer.
		 */
		switch (recieved)
		{
		case CONFIRM_BYTE:
			UART_sendByte (userChoice);
			TIMER1_setCallBack (timerCallBack_15Sec);
			TIMER1_init (&s_timerConfigurations_15Sec);
			LCD_clearScreen ();
			LCD_moveCursor (0,5);
			LCD_displayString ("DOOR IS");
			LCD_moveCursor (1,4);
			LCD_displayString ("UNLOCKING");
			g_matchingFlag = 'd';
			break;

		case WRONG_BYTE:
			TIMER1_setCallBack (timerCallBack_60Sec);
			TIMER1_init (&s_timerConfigurations_60Sec);
			LCD_clearScreen ();
			LCD_moveCursor (0,5);
			LCD_displayString ("THIEF!");
			g_matchingFlag = 'z';
		}
		break;

	case '-':
		repeatPassword ();
		recieved = UART_recieveByte ();
		/* Depending on the received byte:
		 * 1. If confirm, change the password.
		 * 2. If wrong after 3 iterations, open the buzzer.
		 */
		switch (recieved)
		{
		case CONFIRM_BYTE:
			UART_sendByte (userChoice);
			g_matchingFlag = WRONG_BYTE;                /* For start to take new password */
			break;

		case WRONG_BYTE:
			TIMER1_setCallBack (timerCallBack_60Sec);
			TIMER1_init (&s_timerConfigurations_60Sec);
			LCD_clearScreen ();
			LCD_moveCursor (0,5);
			LCD_displayString ("THIEF!");
			g_matchingFlag = 'z';
		}
	}
}

/*
 * Description:
 * 1. Take the confirmation password for taking the user's action and send it to control_ECU.
 */
void repeatPassword (void)
{
	uint8 i = 0;

	LCD_clearScreen();
	LCD_displayString ("PLZ ENTER PASS:");
	LCD_moveCursor (1,0);

	for (i = 0; i <= 5; i++)
	{
		g_definedPassArray [i] = KEYPAD_getPressedKey ();
		if (g_definedPassArray [i] == 13)
		{
			break;
		}
		LCD_sendData ('*');
		_delay_ms (450);
	}
	g_definedPassArray[i] = '#';				/* For UART_recieveString function */
	g_definedPassArray[i+1] = '\0';				/* For UART_sendString function */
	UART_sendString (g_definedPassArray);
}
