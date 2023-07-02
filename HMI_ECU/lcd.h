/******************************************************************************
 *
 * Module: LCD
 *
 * File Name: lcd.h
 *
 * Author: Mohamed Nasser
 *
 * Date Created: Sep 30, 2022
 *
 * Description: Header file for the LCD Module driver
 *
 *******************************************************************************/

#ifndef LCD_H_
#define LCD_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

/* LCD_DATA_MODES */
#define LCD_BIT_MODE 8
#if (LCD_BIT_MODE != 8 && LCD_BIT_MODE != 4)
#error "The Bit Mode Is Wrong"
#endif

/* Static Configurations */
#define LCD_RS_PORT                          PORTB_ID
#define LCD_RS_PIN                           PIN0_ID

#define LCD_RW_PORT                          PORTB_ID
#define LCD_RW_PIN                           PIN1_ID

#define LCD_EN_PORT                          PORTB_ID
#define LCD_EN_PIN                           PIN2_ID

#define LCD_DATA_PORT                        PORTC_ID

#if (LCD_BIT_MODE == 4)
#define LCD_D4_PIN                           PIN3_ID
#define LCD_D5_PIN                           PIN4_ID
#define LCD_D6_PIN                           PIN5_ID
#define LCD_D7_PIN                           PIN6_ID
#endif

/* LCD_COMMANDS */
#define LCD_4BITS_INIT1                      0x33
#define LCD_4BITS_INIT2                      0x32
#define LCD_4BITS_MODE                       0x28
#define LCD_8BITS_MODE                       0x38
#define DISPLAY_ON_CURSOR_OFF                0x0C
#define CLEAR_DISPLAY                        0x01

/* LCD_MEMORIES */
#define FIRST_ROW_ADDRESS                    0x00
#define SECOND_ROW_ADDRESS                   0x40
#define THIRD_ROW_ADDRESS                    0x10
#define FOURTH_ROW_ADDRESS                   0x50
#define CGROM_ADDRESS                        0x40

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Initialize the LCD:
 * 1. Setup the LCD pins directions by use the GPIO driver.
 * 2. Setup the LCD Data Mode 4-bits or 8-bits.
 */
void LCD_init(void);

/*
 * Description :
 * Send the required command to the screen
 */
void LCD_sendCommand(uint8 command);

/*
 * Description :
 * Display the required character on the screen
 */
void LCD_sendData(uint8 data);

/*
 * Description :
 * Display the required string on the screen
 */
void LCD_displayString(char * ptr);

/*
 * Description :
 * Move the cursor to a specified row and column index on the screen
 */
void LCD_moveCursor(uint8 row, uint8 col);

/*
 * Description :
 * Send the clear screen command
 */
void LCD_clearScreen(void);

/*
 * Description :
 * Display the required decimal value on the screen
 */
void LCD_displayInteger(sint32 num);

/*
 * Description :
 * Create a specific character pattern in the CGROM
 */
void LCD_createCharacter(uint8 location, uint8* character);

#endif /* LCD_H_ */
