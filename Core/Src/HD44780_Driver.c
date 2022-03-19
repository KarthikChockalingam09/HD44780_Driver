/*
 * HD44780_Driver.c
 *
 *  Created on: Feb 12, 2022
 *      Author: Karthik
 */


#include "HD44780_Driver.h"

// Global variable
volatile uint8_t u8IntfMode = LCD_8BIT_INTERFACE_MODE;


/**
  * @brief  Writes the 8 bit data onto the LCD Data bus
  *
  * @note   This function updates the respective bits of data port's ODR
  * 		*GPIO pins* to connect LCD data port should be from same GPIO peripheral instance
  * 		This function is not used in case of LCD connection via I2C
  *
  * @param  data - 8bit data to be written onto the databus
  * @retval None
  */
void LCD_WriteToDatabus(uint8_t u8Data)
{
#ifndef LCD_I2C_INTERFACE_MODE
	uint16_t u16TempData;
	u16TempData = LCD_DATA_PORT->ODR;
	u16TempData &= (0xFF << LCD_DATABUS_LEN);
	u16TempData |= u8Data;
	LCD_DATA_PORT->ODR = u16TempData;
#endif
}

/**
  * @brief  Sends command to the LCD controller
  *
  * @note   This function sets the control pins LCD_RS_PIN and LCD_RW_PIN(RS - LOW, RW - LOW) to send command
  *         And, then writes the command on the data bus.
  *         Finally, enable the LCD controller to process the given command.
  *
  * @param command - 8bit command that needs to be sent
  * @retval None
  */
void LCD_WriteCommand(uint8_t u8Command)
{
#ifndef LCD_I2C_INTERFACE_MODE
	if(u8IntfMode == LCD_8BIT_INTERFACE_MODE)
	{
	    HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_EN_PIN, RESET);
	    HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_RS_PIN, RESET);
	    HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_RW_PIN, RESET);
	    LCD_WriteToDatabus(u8Command);
	    HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_EN_PIN, SET);
	    HAL_Delay(LCD_PROCESS_DELAY);
	    HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_EN_PIN, RESET);
	}
	else
	{
	    uint8_t u8UpperNibble, u8LowerNibble;
	    u8UpperNibble = (0xF0 & u8Command);
	    u8LowerNibble = ((u8Command & 0x0F) << 4);

	    HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_EN_PIN, RESET);
	    HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_RS_PIN, RESET);

		LCD_WriteToDatabus(u8UpperNibble);
	    HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_EN_PIN, SET);
	    HAL_Delay(LCD_PROCESS_DELAY);
	    HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_EN_PIN, RESET);

		LCD_WriteToDatabus(u8LowerNibble);
	    HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_EN_PIN, SET);
	    HAL_Delay(LCD_PROCESS_DELAY);
	    HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_EN_PIN, RESET);
	}
#else
    uint8_t u8UpperNibble, u8LowerNibble;
    uint8_t u8SendData[4];
    u8UpperNibble = (0xF0 & u8Command);
    u8LowerNibble = ((u8Command & 0x0F) << 4);

    //I2C-Parallel converter to LCD pin mapping
	//I2C-Parallel Port -> P7 P6 P5 P4 P3 P2 P1 P0
	//LCD Parallel Port -> D7 D6 D5 D4 -  E  RW RS

    u8SendData[0] = (u8UpperNibble | 0x0C); //Send upper nibble first with EN as high
    u8SendData[1] = (u8UpperNibble | 0x08); //Then, send upper nibble with  EN as low
    u8SendData[2] = (u8LowerNibble | 0x0C); //Next, send lower nibble with EN as high
    u8SendData[3] = (u8LowerNibble | 0x08); //Then, send upper nibble with EN as low

	HAL_I2C_Master_Transmit(&hi2c1, LCD_I2C_WRITE_ADDRESS, u8SendData, 4, HAL_MAX_DELAY);

#endif
}

/**
  * @brief  Sends data to the LCD controller
  *
  * @note   This function sets the control pins LCD_RS_PIN and LCD_RW_PIN(RS - HIGH, RW - LOW) to send data
  *         And, then writes the data on the data bus.
  *         Finally, enable the LCD controller to process the given data.
  *
  * @param data - pointer to data that needs to be sent
  * @param len - string length of the data
  * @retval None
  */
void LCD_WriteData(uint8_t* pu8Data, uint32_t u32Len)
{
#ifndef LCD_I2C_INTERFACE_MODE
	if(u8IntfMode == LCD_8BIT_INTERFACE_MODE)
	{
	    HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_RS_PIN, SET);
	    HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_RW_PIN, RESET);
	    while(u32Len > 0)
	    {
	        LCD_WriteToDatabus(*pu8Data);
	        pu8Data++;
	        u32Len--;

	        HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_EN_PIN, SET);
	        HAL_Delay(LCD_PROCESS_DELAY);
	        HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_EN_PIN, RESET);
	    }
	    HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_RS_PIN, RESET);
	}
	else
	{
	    HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_RS_PIN, SET);
	    HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_RW_PIN, RESET);

		while(u32Len > 0)
		{
			uint8_t u8UpperNibble, u8LowerNibble;
			u8UpperNibble = (0xF0 & *pu8Data);
			u8LowerNibble = ((*pu8Data & 0x0F) << 4);

	        LCD_WriteToDatabus(u8UpperNibble);
	        HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_EN_PIN, SET);
	        HAL_Delay(LCD_PROCESS_DELAY);
	        HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_EN_PIN, RESET);

	        LCD_WriteToDatabus(u8LowerNibble);
	        HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_EN_PIN, SET);
	        HAL_Delay(LCD_PROCESS_DELAY);
	        HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_EN_PIN, RESET);

	        pu8Data++;
	        u32Len--;
		}
		HAL_GPIO_WritePin(LCD_CMD_PORT, LCD_RS_PIN, RESET);
	}
#else
	while(u32Len > 0)
	{
		uint8_t u8UpperNibble, u8LowerNibble;
		uint8_t u8SendData[4];
		u8UpperNibble = (0xF0 & *pu8Data);
		u8LowerNibble = ((*pu8Data & 0x0F) << 4);

		//I2C-Parallel converter to LCD pin mapping
		//I2C-Parallel Port -> P7 P6 P5 P4 P3 P2 P1 P0
		//LCD Parallel Port -> D7 D6 D5 D4 -  E  RW RS

		// RS - High for writing data
		u8SendData[0] = (u8UpperNibble | 0x0D); //Send upper nibble first with EN as high
		u8SendData[1] = (u8UpperNibble | 0x09); //Then, send upper nibble with  EN as low
		u8SendData[2] = (u8LowerNibble | 0x0D); //Next, send lower nibble with EN as high
		u8SendData[3] = (u8LowerNibble | 0x09); //Then, send upper nibble with EN as low

		HAL_I2C_Master_Transmit(&hi2c1, LCD_I2C_WRITE_ADDRESS, u8SendData, 4, HAL_MAX_DELAY);

		pu8Data++;
        u32Len--;
	}
#endif
}

/**
  * @brief  Clears the LCD screen
  *
  * @param  None
  * @retval None
  */
void LCD_ClearScreen(void)
{
    LCD_WriteCommand(LCD_CLEARSCREEN);
    HAL_Delay(LCD_COMMAND_DELAY);
}

/**
  * @brief  configures the cursor mode - blink, not to blink or off
  *
  * @param  ctrl - selects the cursor mode
  *                possible values - @ LCD cursor mode macros
  * @retval None
  */

void LCD_CursorControl(uint8_t u8Ctrl)
{
    if(u8Ctrl == LCD_CURSOR_OFF)
    {
        LCD_WriteCommand(LCD_DISPLAY_ON_CURSOR_OFF);
        HAL_Delay(LCD_COMMAND_DELAY);
    }
    else if(u8Ctrl == LCD_CURSOR_BLINK)
    {
        LCD_WriteCommand(LCD_DISPLAY_ON_CURSOR_BLINK);
        HAL_Delay(LCD_COMMAND_DELAY);
    }
    else
    {
        LCD_WriteCommand(LCD_DISPLAY_ON_CURSOR_NOBLINK);
        HAL_Delay(LCD_COMMAND_DELAY);
    }
}

/**
  *
  * @brief   Sets cursor onto the beginning of the line
  *
  * @param  line - selects line1 or line2 of LCD display
  *         possible values - @ LCD line select macros
  * @retval None
  */

void LCD_MoveCursorToBegin(uint8_t u8Line)
{
    if(u8Line == LCD_LINE1)
    {
        LCD_WriteCommand(LCD_CURSOR_TO_BEGIN_LINE1);
        HAL_Delay(LCD_COMMAND_DELAY);
    }
    else
    {
        LCD_WriteCommand(LCD_CURSOR_TO_BEGIN_LINE2);
        HAL_Delay(LCD_COMMAND_DELAY);
    }

}

/**
  * @brief  Sets the cursor to a specific position
  *
  * @param  line - selects line1 or line2 of LCD display
  *                possible values - @LCD line select macros)
  *
  * @param  pos - denotes cursor position in corresponding line of LCD display
  *               possible values - @LCD cursor position macros
  * @retval None
  */

void LCD_SetCursor(uint8_t u8Line, uint8_t u8Pos)
{
    if(u8Line == LCD_LINE1)
    {
        LCD_WriteCommand(LCD_LINE1_CURSORBASE+u8Pos);
        HAL_Delay(LCD_COMMAND_DELAY);
    }
    else
    {
        LCD_WriteCommand(LCD_LINE2_CURSORBASE+u8Pos);
        HAL_Delay(LCD_COMMAND_DELAY);
    }
}

/**
  * @brief  Displays data on first line of the LCD
  *
  * @note   This function clears the LCD screen, moves the cursor to beginning of line and
  *         writes data to the LCD controller
  *
  * @param data - pointer to data that needs to be displayed
  * @retval None
  */

void LCD_Display1Page(uint8_t* pu8StrData)
{
    LCD_ClearScreen();
    LCD_MoveCursorToBegin(LCD_LINE1);
    LCD_WriteData(pu8StrData, strlen((char*)pu8StrData));
}

/**
  * @brief  Displays data on first & second line of the LCD
  *
  * @note   This function clears the LCD screen, moves the cursor to beginning of line respectively and
  *         writes data to the LCD controller
  *
  * @param data - pointer to data that needs to be displayed
  * @retval None
  */

void LCD_Display2Page(uint8_t* pu8StrData[])
{
    LCD_ClearScreen();
    LCD_MoveCursorToBegin(LCD_LINE1);
    LCD_WriteData(pu8StrData[0], strlen((char*)pu8StrData[0]));
    LCD_MoveCursorToBegin(LCD_LINE2);
    LCD_WriteData(pu8StrData[1], strlen((char*)pu8StrData[1]));
}

/**
  * @brief  Initializes the LCD by setting entry function,
  *         clears screen and sets cursor mode
  *
  * @param  None
  * @retval None
  */

void LCD_Init(void)
{
	HAL_Delay(150);  						// wait for more than 100ms

	LCD_WriteCommand(0x30);					// Special case of 'Function set', lower four bits are irrelevant
	HAL_Delay(5);  							// wait for >4.1ms

	LCD_WriteCommand(0x30);					// Special case of 'Function set', lower four bits are irrelevant
	HAL_Delay(LCD_COMMAND_DELAY);  			// wait for >100us

	LCD_WriteCommand(0x30);					// Special case of 'Function set', lower four bits are irrelevant
	HAL_Delay(LCD_COMMAND_DELAY);			// wait for >100us

#if LCD_INTERFACE_MODE

	LCD_WriteCommand(0x20);  				// Initial function set to change interface, lower four bits are irrelevant
	HAL_Delay(LCD_COMMAND_DELAY);			// wait for >100us

	// Device is in 8-bit mode when powered up and it remains same, until the last command.

	u8IntfMode = LCD_4BIT_INTERFACE_MODE;
	// Function set --> DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
	// Function set --> 0   0   1   DL   N   F   -   -
	// DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters) -> 0x28
	LCD_WriteCommand(LCD_FUNCTION_SETUP_2);
	HAL_Delay(LCD_COMMAND_DELAY);

#else

	u8IntfMode = LCD_8BIT_INTERFACE_MODE;

	// DL=1 (8 bit mode), N = 1 (2 line display) F = 0 (5x8 characters) -> 0x38
	LCD_WriteCommand(LCD_FUNCTION_SETUP_1);
	HAL_Delay(LCD_COMMAND_DELAY);

#endif

	// Display on/off --> DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
	// Display on/off --> 0   0   0   0   1   D   C   B
	// D=0,C=0, B=0  ---> display off, cursor off, cursor blink off  -> 0x08
	LCD_WriteCommand(0x08);
	HAL_Delay(LCD_COMMAND_DELAY);

	LCD_WriteCommand(0x01);  		// clear display -> 0x01
	HAL_Delay(5);

	// Entry Mode set --> DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
	// Entry Mode set --> 0   0   0   0   0   1   I/D  S
	// /D = 1 (increment cursor) & S = 0 (no shift)  -> 0x06
	LCD_WriteCommand(0x06);
	HAL_Delay(LCD_COMMAND_DELAY);

	//Display on/off control --> D, C and B = 1. (Display, Cursor and blink -> ON)
	LCD_WriteCommand(0x0F);
	HAL_Delay(LCD_COMMAND_DELAY);

    LCD_ClearScreen();
    LCD_CursorControl(LCD_CURSOR_OFF);
}
