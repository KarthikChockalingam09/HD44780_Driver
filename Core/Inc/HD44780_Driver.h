/*
 * HD44780_Driver.h
 *
 *  Created on: Feb 12, 2022
 *      Author: Karthik
 */

#ifndef INC_HD44780_DRIVER_H_
#define INC_HD44780_DRIVER_H_

#include "stm32f4xx.h"
#include<string.h>

/**********************************************************************************************/

/*
 * @ User settings - GPIO pins, Interface Mode
 */

//refer @LCD interface mode macros
#define LCD_INTERFACE_MODE				LCD_4BIT_INTERFACE_MODE

//@ LCD - connection via I2C macros
#define LCD_I2C_INTERFACE_MODE     		// Uncomment when I2C is not used

#ifndef LCD_I2C_INTERFACE_MODE

//@ LCD GPIO port macros

#define LCD_DATA_PORT            GPIOB
#define LCD_CMD_PORT             GPIOC

//@ LCD GPIO pin macros

#define LCD_RS_PIN 			GPIO_PIN_8
#define LCD_EN_PIN 			GPIO_PIN_6
#define LCD_RW_PIN 			GPIO_PIN_10

#define LCD_D0_PIN 			GPIO_PIN_0
#define LCD_D1_PIN 			GPIO_PIN_1
#define LCD_D2_PIN 			GPIO_PIN_2
#define LCD_D3_PIN 			GPIO_PIN_3
#define LCD_D4_PIN 			GPIO_PIN_4
#define LCD_D5_PIN 			GPIO_PIN_5
#define LCD_D6_PIN 			GPIO_PIN_6
#define LCD_D7_PIN 			GPIO_PIN_7

#else

extern I2C_HandleTypeDef hi2c1;

#define LCD_I2C_SDA_PIN			GPIO_PIN_7
#define LCD_I2C_SDA_PORT		GPIOB
#define LCD_I2C_SCL_PIN			GPIO_PIN_6
#define LCD_I2C_SCL_PORT		GPIOB

#define LCD_I2C_BASE_ADDRESS	0x27
#define LCD_I2C_READ_ADDRESS	0x4F
#define LCD_I2C_WRITE_ADDRESS	0x4E

#endif
/**********************************************************************************************/

/*
 * @ LCD delay macros
 */

#define LCD_PROCESS_DELAY                   15
#define LCD_COMMAND_DELAY                   1

/*
 * @ LCD interface mode macros
 */

#define LCD_8BIT_INTERFACE_MODE              0
#define LCD_4BIT_INTERFACE_MODE              1

/*
 * @ LCD command macros
 */

#define LCD_CLEARSCREEN                     0x01
#define LCD_RETURNHOME                      0x02
#define LCD_CURSOR_TO_LEFT                  0x10
#define LCD_CURSOR_TO_RIGHT                 0x14
#define LCD_DISPLAY_OFF_CURSOR_OFF          0x08
#define LCD_DISPLAY_OFF_CURSOR_ON           0x0A
#define LCD_DISPLAY_ON_CURSOR_OFF           0x0C
#define LCD_DISPLAY_ON_CURSOR_NOBLINK       0x0E
#define LCD_DISPLAY_ON_CURSOR_BLINK         0x0F
#define LCD_CURSOR_TO_BEGIN_LINE1           0x80
#define LCD_CURSOR_TO_BEGIN_LINE2           0xC0
#define LCD_FUNCTION_SETUP_1                0x38        // 2lines, 5x8 dot matrix, 8bit mode
#define LCD_FUNCTION_SETUP_2                0x28        // 2lines, 5x8 dot matrix, 4bit mode

/*
 * @ LCD line base address macros
 */

#define LCD_LINE1_CURSORBASE    0x80U
#define LCD_LINE2_CURSORBASE    0xC0U


/*
 * @ LCD line selection macros
 */

#define LCD_LINE1               0
#define LCD_LINE2               1

/*
 * @ LCD cursor position macros
 */

#define LCD_CURSOR_POS1         0
#define LCD_CURSOR_POS2         1
#define LCD_CURSOR_POS3         2
#define LCD_CURSOR_POS4         3
#define LCD_CURSOR_POS5         4
#define LCD_CURSOR_POS6         5
#define LCD_CURSOR_POS7         6
#define LCD_CURSOR_POS8         7
#define LCD_CURSOR_POS9         8
#define LCD_CURSOR_POS10        9
#define LCD_CURSOR_POS11        10
#define LCD_CURSOR_POS12        11
#define LCD_CURSOR_POS13        12
#define LCD_CURSOR_POS14        13
#define LCD_CURSOR_POS15        14
#define LCD_CURSOR_POS16        15

/*
 * @ LCD cursor mode macros
 */

#define LCD_CURSOR_OFF          0
#define LCD_CURSOR_BLINK        1
#define LCD_CURSOR_NOBLINK      2

/*
 * @ LCD general macros
 */

#define SET                     1
#define RESET                   0

#define LCD_DATABUS_LEN         8
#define LCD_DISPLAY_LEN         16

/**********************************************************************************************/

// Function prototypes
void LCD_WriteToDatabus(uint8_t u8Data);
void LCD_WriteCommand(uint8_t u8Command);
void LCD_WriteData(uint8_t* pu8Data, uint32_t u32Len);
void LCD_CursorControl(uint8_t u8Ctrl);
void LCD_MoveCursorToBegin(uint8_t u8Line);
void LCD_SetCursor(uint8_t u8Line, uint8_t u8Pos);
void LCD_ClearScreen(void);
void LCD_Display2Page(uint8_t* pu8StrData[]);
void LCD_Display1Page(uint8_t* pu8StrData);
void LCD_Init(void);

#endif /* INC_HD44780_DRIVER_H_ */

