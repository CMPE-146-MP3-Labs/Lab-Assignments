#include "ssd1309.h"
#include "gpio.h"
#include "lpc40xx.h"
#include "ssp2.h"

void LCD_CS(void) { gpio__reset(CS); };

void LCD_DS(void) { gpio__set(CS); };

void LCD_Command_Mode(void) { gpio__reset(DC); }

void LCD_Data_Mode(void) { gpio__set(DC); }

void LCD_Write(uint8_t a_byte) { ssp2__exchange_byte(a_byte); }

void LCD_Init() {
  LCD_Command_Mode();
  gpio__reset(VCC);
  LCD_CS();
  gpio__reset(RESET);
  /* this for loop is a placeholder for a sleep function */
  for (int i = 0; i < 50; i++) {
  };
  gpio__set(RESET);
  gpio__set(VCC);
  LCD_Write(CMD_Set_Display_On);
  LCD_Set_Contrast(0xFF);
  LCD_clear();
  LCD_DS();
}

void LCD_Set_Contrast(uint8_t contrast_value) {
  LCD_Command_Mode();
  LCD_Write(CMD_Set_Contrast_Control);
  LCD_Write(contrast_value);
}

void LCD_Set_Normal_Display(void) {
  LCD_Command_Mode();
  LCD_Write(CMD_Set_Normal_Display);
}

void LCD_Set_Inverse_Display(void) {
  LCD_Command_Mode();
  LCD_Write(CMD_Set_Inverse_Display);
}

void LCD_Cont_Horiz_Scroll_Setup(uint8_t direction, uint8_t start_page_address,
                                 uint8_t time_interval,
                                 uint8_t end_page_address, uint8_t start_column,
                                 uint8_t end_column_address) {
  uint8_t scroll_direction = (0b00100110 | direction);
  uint8_t dummy_byte = 0x00;
  LCD_Command_Mode();
  LCD_Write(CMD_Deactivate_Scroll);
  LCD_Write(scroll_direction);
  LCD_Write(dummy_byte);
  LCD_Write(start_page_address);
  LCD_Write(time_interval);
  LCD_Write(end_page_address);
  LCD_Write(dummy_byte);
  LCD_Write(start_column);
  LCD_Write(end_column_address);
  LCD_Write(CMD_Activate_Scroll);
};

void CMD_Set_Lower_Column_Start_Address(uint8_t lower_column_start_address) {
  uint8_t mask = 0x00;
  mask = mask | lower_column_start_address;
  LCD_Write(mask);
}

void CMD_Set_Higher_Column_Start_Address(uint8_t higher_column_start_address) {
  uint8_t mask = 0x10;
  mask = mask | higher_column_start_address;
  LCD_Write(mask);
}

void CMD_Set_Column_Start_Address(uint8_t column_start_address) {
  uint8_t lower_nibble = (column_start_address & 0x0F);
  uint8_t higher_nible = (column_start_address >> 4);
  CMD_Set_Lower_Column_Start_Address(lower_nibble);
  CMD_Set_Higher_Column_Start_Address(higher_nible);
}

void CMD_Set_Page_Start_Address(uint8_t page_start_address) {
  uint8_t mask = 0xB0;
  mask = mask | page_start_address;
  LCD_Write(mask);
}

void CMD_Set_Display_Start_Line(uint8_t start_line_register) {
  uint8_t mask = 0b11000000;
  mask = mask | start_line_register;
  LCD_Write(mask);
}

void LCD_Set_Starting_RAM_Access_Pointer(uint8_t page_start_address,
                                         uint8_t column_start_address) {
  LCD_Command_Mode();
  CMD_Set_Page_Start_Address(page_start_address);
  CMD_Set_Column_Start_Address(column_start_address);
  LCD_Data_Mode();
}

void LCD_clear(void) {
  for (int i = 0; i <= PAGE_MAX; i++) {
    LCD_Command_Mode();
    LCD_Set_Starting_RAM_Access_Pointer(i, 0);

    for (int j = 0; j <= COL_MAX; j++) {
      LCD_Write(0x00);
    }
  }
}