#pragma once

#include <linux/types.h>

typedef enum
{
    LCD1602A_INTERFACE_MODE_COMMAND = 0,
    LCD1602A_INTERFACE_MODE_DATA = 1
} lcd1602a_interface_mode_t;

typedef enum
{
    LCD1602A_DATA_LENGTH_4_BIT = 0,
    LCD1602A_DATA_LENGTH_8_BIT = 1
} lcd1602a_data_length_t;

typedef enum
{
    LCD1602A_FONT_SIZE_5x8 = 0,
    LCD1602A_FONT_SIZE_5x10 = 1
} lcd1602a_font_size_t;

bool lcd1602a_interface_initialize(void);
void lcd1602a_interface_deinitialize(void);

void lcd1602a_interface_send_data(lcd1602a_interface_mode_t mode, uint8_t data);

void lcd1602a_interface_clear(void);
void lcd1602a_interface_return_home(void);
void lcd1602a_interface_set_entry_mode(bool increment, bool shift);
void lcd1602a_interface_set_display_control(bool display_on, bool cursor_on, bool cursor_position_on);
void lcd1602a_interface_set_cursor_display_shift(bool increment, bool direction);
void lcd1602a_interface_set_function_set(lcd1602a_data_length_t data_length, uint8_t number_of_lines, lcd1602a_font_size_t font_size);
void lcd1602a_interface_set_cdram_addres(uint8_t address);
void lcd1602a_interface_set_ddram_addres(uint8_t address);
void lcd1602a_interface_write_to_ram(uint8_t value);
