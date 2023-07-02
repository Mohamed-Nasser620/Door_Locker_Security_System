/******************************************************************************
 *
 * Module: LCD
 *
 * File Name: lcd.c
 *
 * Author: Mohamed Nasser
 *
 * Date Created: Sep 30, 2022
 *
 * Description: Source file for the LCD Module driver
 *
 *******************************************************************************/

#include <stdlib.h>
#include <util/delay.h>
#include "common_macros.h"
#include "LCD.h"
#include "gpio.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Initialize the LCD:
 * 1. Setup the LCD pins directions by use the GPIO driver.
 * 2. Setup the LCD Data Mode 4-bits or 8-bits.
 */
void LCD_init(void)
{
	/* Configure the direction for RS and E pins as output pins */
	GPIO_setupPinDirection (LCD_RS_PORT, LCD_RS_PIN, PIN_OUTPUT);
	GPIO_setupPinDirection (LCD_RW_PORT, LCD_RW_PIN, PIN_OUTPUT);
	GPIO_setupPinDirection (LCD_EN_PORT, LCD_EN_PIN, PIN_OUTPUT);
	GPIO_writePin (LCD_RW_PORT, LCD_RW_PIN, LOGIC_LOW);

#if (LCD_BIT_MODE == 4)
	/* Configure 4 pins in the data port as output pins */
	GPIO_setupPinDirection (LCD_DATA_PORT, LCD_D4_PIN, PIN_OUTPUT);
	GPIO_setupPinDirection (LCD_DATA_PORT, LCD_D5_PIN, PIN_OUTPUT);
	GPIO_setupPinDirection (LCD_DATA_PORT, LCD_D6_PIN, PIN_OUTPUT);
	GPIO_setupPinDirection (LCD_DATA_PORT, LCD_D7_PIN, PIN_OUTPUT);

	/* use 2-line lcd + 4-bit Data Mode + 5*7 dot display Mode */
	LCD_sendCommand (LCD_4BITS_INIT1);
	LCD_sendCommand (LCD_4BITS_INIT2);
	LCD_sendCommand (LCD_4BITS_MODE);

#elif (LCD_BIT_MODE == 8)
	/* Configure the data port as output port */
	GPIO_setupPortDirection (LCD_DATA_PORT, PORT_OUTPUT);

	/* use 2-line lcd + 8-bit Data Mode + 5*7 dot display Mode */
	LCD_sendCommand (LCD_8BITS_MODE);
#endif

	LCD_sendCommand(DISPLAY_ON_CURSOR_OFF);
	LCD_sendCommand(CLEAR_DISPLAY);            /* Clear LCD at the beginning */
}

/*
 * Description :
 * Send the required command to the screen
 */
void LCD_sendCommand(uint8 command)
{
	/* Instruction Mode RS=0 */
	GPIO_writePin(LCD_RS_PORT, LCD_RS_PIN, LOGIC_LOW);
	_delay_ms(1);            /* delay for processing Tas = 50ns */

	GPIO_writePin(LCD_EN_PORT, LCD_EN_PIN, LOGIC_HIGH);
	_delay_ms(1);            /* delay for processing Tpw - Tdws = 190ns */

#if (LCD_BIT_MODE == 4)
	/* out the last 4 bits of the required command to the data bus D4 --> D7 */
	GPIO_writePin(LCD_DATA_PORT, LCD_D4_PIN, GET_BIT (command, 4));
	GPIO_writePin(LCD_DATA_PORT, LCD_D5_PIN, GET_BIT (command, 5));
	GPIO_writePin(LCD_DATA_PORT, LCD_D6_PIN, GET_BIT (command, 6));
	GPIO_writePin(LCD_DATA_PORT, LCD_D7_PIN, GET_BIT (command, 7));
	_delay_ms(1);            /* delay for processing Tdsw = 100ns */

	GPIO_writePin(LCD_EN_PORT, LCD_EN_PIN, LOGIC_LOW);
	_delay_ms(1);            /* delay for processing Th = 13ns */

	GPIO_writePin(LCD_EN_PORT, LCD_EN_PIN, LOGIC_HIGH);
	_delay_ms(1);            /* delay for processing Tpw - Tdws = 190ns */

	/* out the first 4 bits of the required command to the data bus D4 --> D7 */
	GPIO_writePin(LCD_DATA_PORT, LCD_D4_PIN, GET_BIT (command, 0));
	GPIO_writePin(LCD_DATA_PORT, LCD_D5_PIN, GET_BIT (command, 1));
	GPIO_writePin(LCD_DATA_PORT, LCD_D6_PIN, GET_BIT (command, 2));
	GPIO_writePin(LCD_DATA_PORT, LCD_D7_PIN, GET_BIT (command, 3));
	_delay_ms(1);           /* delay for processing Tdsw = 100ns */

#elif (LCD_BIT_MODE == 8)
	/* out the required command to the data bus D0 --> D7 */
	GPIO_writePort(LCD_DATA_PORT, command);
	_delay_ms(1);           /* delay for processing Tdsw = 100ns */
#endif

	GPIO_writePin(LCD_EN_PORT, LCD_EN_PIN, LOGIC_LOW);
	_delay_ms(1);           /* delay for processing Th = 13ns */
}

/*
 * Description :
 * Display the required character on the screen
 */
void LCD_sendData(uint8 data)
{
	/* Data Mode RS=1 */
	GPIO_writePin(LCD_RS_PORT, LCD_RS_PIN, LOGIC_HIGH);
	_delay_ms(1);            /* delay for processing Tas = 50ns */

	GPIO_writePin(LCD_EN_PORT, LCD_EN_PIN, LOGIC_HIGH);
	_delay_ms(1);            /* delay for processing Tpw - Tdws = 190ns */

#if (LCD_BIT_MODE == 4)
	/* out the last 4 bits of the required data to the data bus D4 --> D7 */
	GPIO_writePin(LCD_DATA_PORT, LCD_D4_PIN, GET_BIT (data, 4));
	GPIO_writePin(LCD_DATA_PORT, LCD_D5_PIN, GET_BIT (data, 5));
	GPIO_writePin(LCD_DATA_PORT, LCD_D6_PIN, GET_BIT (data, 6));
	GPIO_writePin(LCD_DATA_PORT, LCD_D7_PIN, GET_BIT (data, 7));
	_delay_ms(1);            /* delay for processing Tdsw = 100ns */

	GPIO_writePin(LCD_EN_PORT, LCD_EN_PIN, LOGIC_LOW);
	_delay_ms(1);            /* delay for processing Th = 13ns */

	GPIO_writePin(LCD_EN_PORT, LCD_EN_PIN, LOGIC_HIGH);
	_delay_ms(1);            /* delay for processing Tpw - Tdws = 190ns */

	/* out the first 4 bits of the required data to the data bus D4 --> D7 */
	GPIO_writePin(LCD_DATA_PORT, LCD_D4_PIN, GET_BIT (data, 0));
	GPIO_writePin(LCD_DATA_PORT, LCD_D5_PIN, GET_BIT (data, 1));
	GPIO_writePin(LCD_DATA_PORT, LCD_D6_PIN, GET_BIT (data, 2));
	GPIO_writePin(LCD_DATA_PORT, LCD_D7_PIN, GET_BIT (data, 3));
	_delay_ms(1);            /* delay for processing Tdsw = 100ns */

#elif (LCD_BIT_MODE == 8)
	/* out the required data to the data bus D0 --> D7 */
	GPIO_writePort(LCD_DATA_PORT, data);
	_delay_ms(1);           /* delay for processing Tdsw = 100ns */
#endif

	GPIO_writePin(LCD_EN_PORT, LCD_EN_PIN, LOGIC_LOW);
	_delay_ms(1);           /* delay for processing Th = 13ns */
}

/*
 * Description :
 * Display the required string on the screen
 */
void LCD_displayString(char * ptr)
{
	uint8 i;
	for(i = 0; ptr[i] != '\0'; i++)
	{
		LCD_sendData(ptr[i]);
	}
}

/*
 * Description :
 * Move the cursor to a specified row and column index on the screen
 */
void LCD_moveCursor(uint8 row, uint8 col)
{
	uint8 position = FIRST_ROW_ADDRESS;

	/* Calculate the required address in the LCD DDRAM */
	switch (row)
	{
	case 0:
		position = FIRST_ROW_ADDRESS + col;
		break;
	case 1:
		position = SECOND_ROW_ADDRESS + col;
		break;
	case 2:
		position = THIRD_ROW_ADDRESS + col;
		break;
	case 3:
		position = FOURTH_ROW_ADDRESS + col;
	}
	/* Move the LCD cursor to this specific address */
	SET_BIT (position, PIN7_ID);
	LCD_sendCommand(position);
}

/*
 * Description :
 * Send the clear screen command
 */
void LCD_clearScreen(void)
{
	LCD_sendCommand (CLEAR_DISPLAY);   /* Send clear display command */
}

/*
 * Description :
 * Display the required decimal value on the screen
 */
void LCD_displayInteger(sint32 num)
{
	char buffer [16] = {0};           /* String to hold the ascii result */
	itoa (num, buffer, 10);           /* Convert the data to its corresponding ASCII value, 10 for decimal */
	LCD_displayString(buffer);
}

/*
 * Description :
 * Create a specific character pattern in the CGROM
 */
void LCD_createCharacter(uint8 location, uint8* character)
{
	uint8 i = 0;
	/* Move the LCD to the first byte address for the new character block in CGROM */
	LCD_sendCommand (CGROM_ADDRESS + (location*8));

	/* Fill the CGROM block byte by byte with the pattern of the required character */
	for (i = 0; i < 8; i++)
	{
		LCD_sendData (character[i]);
	}
}
