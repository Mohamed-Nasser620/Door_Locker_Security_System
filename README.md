# Door_Locker_Security_System
C Project - Based on Atmega32 Microcontroller
- Developed a system that takes a password and confirm it using keypad and displays '*' on LCD to start the system for the first time, 
all of that is done in HMI_ECU, then this password is sent to control-ECU using UART to be stored in external EEPROM to compare with it when the users want to open the door
and if the password is entered wrong for 3 times a buzzer will start for 1 minute.
- Drivers: GPIO, Keypad, LCD, Timer, UART, I2C, EEPROM, Buzzer and DC-Motor
