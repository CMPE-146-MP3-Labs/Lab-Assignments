#pragma once

#include "gpio.h"
#include "lpc40xx.h"
#include "ssp2.h"

#define RESET gpio__construct_as_output(GPIO__PORT_2, 0)
#define VCC gpio__construct_as_output(GPIO__PORT_2, 1)
#define CS gpio__construct_as_output(GPIO__PORT_2, 2)
#define DC gpio__construct_as_output(GPIO__PORT_2, 4)

/* LCD Macros */
#define COL_MAX 127
#define COL_MIN 0
#define ROW_MAX 63
#define ROW_MIN 0
#define PAGE_MAX 7
#define PAGE_MIN 0
#define PAGE0 0
#define PAGE1 1
#define PAGE2 2
#define PAGE3 3
#define PAGE4 4
#define PAGE5 5
#define PAGE6 6
#define PAGE7 7

#define OFF 0
#define ON 1

#define RIGHT 0
#define LEFT 1

/* DC pin mode */
#define DATA 1
#define CMD 0

/* Fundamental Command Table */
#define CMD_Set_Contrast_Control 0x81
#define CMD_Entire_Display_On_RAM_Follow 0xA4
#define CMD_Entire_Display_On_RAM_Ignore 0xA5
#define CMD_Set_Normal_Display 0xA6
#define CMD_Set_Inverse_Display 0xA7
#define CMD_Set_Display_Off 0xAE
#define CMD_Set_Display_On 0xAF
#define CMD_NOP 0xE3
#define CMD_Set_Command_Lock 0xFD

/* Scrolling Command Table */
#define CMD_Continuous_Horizontal_Scroll_Right 0x26
#define CMD_Continuous_Horizontal_Scroll_Left 0x27
#define CMD_Continuous_VerticalHorizontal_Scroll_Right 0x29
#define CMD_Continuous_VerticalHorizontal_Scroll_Left 0x2A
#define CMD_Deactivate_Scroll 0x2E
#define CMD_Activate_Scroll 0x2F
#define CMD_Set_Vertical_Scroll_Area 0xA3
#define CMD_Content_Scroll_Right 0x2C
#define CMD_Content_Scroll_Left 0x2D

/* Addressing Setting Command Table */
#define CMD_Set_Lower_Column_Start_Address_default                             \
  0x00 // This value can be 0x00 - 0x0F
#define CMD_Set_Upper_Column_Start_Address_default                             \
  0x10 // This value can be 0x10 - 0x1F
#define CMD_Set_Memory_Addressing_Mode 0x20
#define CMD_Set_Column_Address 0x21
#define CMD_Set_Page_Address 0x22
#define CMD_Set_Page_Start_Address_default 0xB0 // This value can be 0xB0 - 0xB7

/* Hardware Configuration (Panel resolution & layout related) Command Table */
#define CMD_Set_Display_Start_Line_default 0x40 // This value can be 0x40 - 0x7F
#define CMD_Set_Segment_Remap_SEG0_0 0xA0
#define CMD_Set_Segment_Remap_SEG0_127 0xA1
#define CMD_Set_Multiplex_Ratio 0xA8
#define CMD_Set_COM_Output_Scan_Direction_Normal 0xC0
#define CMD_Set_COM_Output_Scan_Direction_Remapped 0xC8
#define CMD_Set_Display_Offset 0xD3
#define CMD_Set_COM_Pins_Hardware_Configuration 0xDA
#define CMD_Set_GPIO 0xDC

/* Timing & Driving Scheme Setting Command Table */
#define CMD_Set_Display_Clock_Ratio 0xD5
#define CMD_Set_Precharge_Period 0xD9
#define CMD_Set_VCOMH_Deselect_Level 0xDB

/* Chip Select*/
void LCD_CS(void);

/* Chip Deselect*/
void LCD_DS(void);

/* Set Command Mode */
void LCD_Command_Mode(void);

/* Set Data Mode */
void LCD_Data_Mode(void);

/** Write Byte to LCD
 * @param  a_byte: 1 byte sent to SSD1309
 */
void LCD_Write(uint8_t a_byte);

/* SSD1309 initialization */
void LCD_Init();

/***** Command Setup Functions *****/

/** Set contrast for the LCD display
 * @param contrast_value : 0 - 255
 */
void LCD_Set_Contrast(uint8_t contrast_value);

/* Set Normal Display
 * 0 in RAM : OFF in display panel
 * 1 in RAM : ON in display panel
 */
void LCD_Set_Normal_Display(void);

/** Set Inverse Display
 * 0 in RAM : ON in display panel
 * 1 in RAM : OFF in display panel
 */
void LCD_Set_Inverse_Display(void);

/** Continuous Horizontal Scroll Setup
 * @param start_page_address
 * Defines the start page address from PAGE0 - PAGE7 (000b - 101b)
 *
 * @param time_interval
 * Time interval between each scroll step in terms of fram frequency
 * 000b - 5   frames        100b - 2 frames
 * 001b - 64  frames        101b - 3 frames
 * 010b - 128 frames        110b - 4 frames
 * 011b - 256 frames        111b - 1 frames
 *
 * @param end_page_address
 * Define end page address PAGE0 - PAGE7 (000b - 101b)
 *
 * @param start_column
 * Define the start column (RESET = 00h)
 *
 * @param end_column_address
 * Define the end column address (RESET = 00h)
 */
void LCD_Cont_Horiz_Scroll_Setup(uint8_t direction, uint8_t start_page_address,
                                 uint8_t time_interval,
                                 uint8_t end_page_address, uint8_t start_column,
                                 uint8_t end_column_address);

/* Page addressing helper functions */

/** Set the lower nibble of the column start addressregister for Page Addressing
 * Mode. The initial display line register is reset to0000b after RESET.
 * @param lower_column_start_address : lower nibble of the column start address
 */
void CMD_Set_Lower_Column_Start_Address(uint8_t lower_column_start_address);

/** Set the higher nibble of the column start address register for Page
 * Addressing Mode. The initial display line register is reset to 0000b after
 * RESET.
 * @param higher_column_start_address : higher nibble of the column start
 * address
 */
void CMD_Set_Higher_Column_Start_Address(uint8_t higher_column_start_address);

/** Set column address. Combines lower nibble with higher nibble of the column
 * start address.
 * @param column_start_address : value 0 - 127
 */
void CMD_Set_Column_Start_Address(uint8_t column_start_address);

/** Set GDDRAM Page Start Address (PAGE0~PAGE7) for Page Addressing Mode
 * @param page_start_address : value 0 - 7 or ( PAGE0, PAGE1, ..., PAGE7)
 */
void CMD_Set_Page_Start_Address(uint8_t page_start_address);

/** Set display RAM display start line register from 0-63.
 * Display start line register is reset to 000000b during RESET.
 * @param start_line_register : value 0 - 63
 */
void CMD_Set_Display_Start_Line(uint8_t start_line_register);

/** Set the starting RAM access pointer location
 * @param page_start_address : value 0 - 7 or ( PAGE0, PAGE1, ..., PAGE7)
 * @param column_start_address : value 0 - 127
 */
void LCD_Set_Starting_RAM_Access_Pointer(uint8_t page_start_address,
                                         uint8_t column_start_address);

/* Clear the LCD Display */
void LCD_clear(void);