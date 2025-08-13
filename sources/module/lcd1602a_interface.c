#include "lcd1602a_interface.h"

#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/delay.h>

#include "bcm2711_gpio_registers.h"
#include "bitwise_ops.h"


#define LCD1602A_INTERFACE_TAS 25
#define LCD1602A_INTERFACE_TPW 140
#define LCD1602A_INTERFACE_TAH 10

bool lcd1602a_interface_initialize(void)
{
    if (!request_region(GPIO_FSEL_0, 3 * sizeof(uint32_t), "lcd1602a"))
        return false;

    uint32_t temp = GPIO_FSEL_0;
    BITFIELD_SET(temp, 15, 3, 1); // D0:GPIO5
    BITFIELD_SET(temp, 18, 3, 1); // D1:GPIO6
    outl(temp, GPIO_FSEL_0_ADDR);

    temp = GPIO_FSEL_1;
    BITFIELD_SET(temp, 9, 3, 1); // D2:GPIO13
    BITFIELD_SET(temp, 18, 3, 1); // D7:GPIO16
    outl(temp, GPIO_FSEL_1_ADDR);

    temp = GPIO_FSEL_2;
    BITFIELD_SET(temp, 6, 3, 1); // RW:GPIO22
    BITFIELD_SET(temp, 9, 3, 1); // D4:GPIO23
    BITFIELD_SET(temp, 12, 3, 1); // D5:GPIO24
    BITFIELD_SET(temp, 16, 3, 1); // D6:GPIO25
    BITFIELD_SET(temp, 21, 3, 1); // E:GPIO27
    outl(temp, GPIO_FSEL_2_ADDR);

    release_region(GPIO_FSEL_0, 3 * sizeof(uint32_t));
    return true;
}

void lcd1602a_interface_deinitialize(void)
{

}

void lcd1602a_interface_send_data(lcd1602a_interface_mode_t rs, uint8_t data)
{

    if (!request_region(GPIO_SET_ADDR, sizeof(uint32_t), "lcd1602a"))
        return;

    uint32_t temp = 0;
    BIT_SET_VALUE(temp, 27, rs); // RS
    BIT_CLEAR(temp, 22); // RW
    BIT_SET_VALUE(temp, 5, BIT_GET(data, 0));
    BIT_SET_VALUE(temp, 6, BIT_GET(data, 1));
    BIT_SET_VALUE(temp, 13, BIT_GET(data, 2));
    BIT_SET_VALUE(temp, 26, BIT_GET(data, 3));
    BIT_SET_VALUE(temp, 23, BIT_GET(data, 4));
    BIT_SET_VALUE(temp, 24, BIT_GET(data, 5));
    BIT_SET_VALUE(temp, 25, BIT_GET(data, 6));
    BIT_SET_VALUE(temp, 16, BIT_GET(data, 7));
    outl(temp, GPIO_SET_ADDR);
    ndelay(LCD1602A_INTERFACE_TAS);

    temp = 0;
    BIT_SET(temp, 27); // E
    outl(temp, GPIO_SET_ADDR);
    ndelay(LCD1602A_INTERFACE_TPW);

    outl(temp, GPIO_CLR_ADDR);
    ndelay(LCD1602A_INTERFACE_TAH);

    release_region(GPIO_SET_ADDR, sizeof(uint32_t));
}

void lcd1602a_interface_clear(void)
{
    lcd1602a_interface_send_data(LCD1602A_INTERFACE_MODE_COMMAND, 0x01);
}

void lcd1602a_interface_return_home(void)
{
    lcd1602a_interface_send_data(LCD1602A_INTERFACE_MODE_COMMAND, 0x02);
}

void lcd1602a_interface_set_entry_mode(bool increment, bool shift)
{
    uint8_t command = 0x04;
    BIT_SET_VALUE(command, 1, increment);
    BIT_SET_VALUE(command, 0, shift);
    lcd1602a_interface_send_data(LCD1602A_INTERFACE_MODE_COMMAND, command);
}

void lcd1602a_interface_set_display_control(bool display_on, bool cursor_on, bool cursor_position_on)
{
    uint8_t command = 0x04;
    BIT_SET_VALUE(command, 2, display_on);
    BIT_SET_VALUE(command, 1, cursor_on);
    BIT_SET_VALUE(command, 0, cursor_position_on);
    lcd1602a_interface_send_data(LCD1602A_INTERFACE_MODE_COMMAND, command);
}

void lcd1602a_interface_set_cursor_display_shift(bool increment, bool direction)
{
    uint8_t command = 0x08;
    BIT_SET_VALUE(command, 3, increment);
    BIT_SET_VALUE(command, 2, direction);
    lcd1602a_interface_send_data(LCD1602A_INTERFACE_MODE_COMMAND, command);
}

void lcd1602a_interface_set_function_set(lcd1602a_data_length_t data_length, uint8_t number_of_lines, lcd1602a_font_size_t font_size)
{
    uint8_t command = 0x20;
    BIT_SET_VALUE(command, 4, data_length);
    BIT_SET_VALUE(command, 3, number_of_lines > 1);
    BIT_SET_VALUE(command, 2, font_size);
    lcd1602a_interface_send_data(LCD1602A_INTERFACE_MODE_COMMAND, command);
}

void lcd1602a_interface_set_cdram_addres(uint8_t address)
{
    uint8_t command = 0x40 | (address & 0x3F);
    lcd1602a_interface_send_data(LCD1602A_INTERFACE_MODE_COMMAND, command);
}

void lcd1602a_interface_set_ddram_addres(uint8_t address)
{
    uint8_t command = 0x80 | (address & 0x7F);
    lcd1602a_interface_send_data(LCD1602A_INTERFACE_MODE_COMMAND, command);
}

void lcd1602a_interface_write_to_ram(uint8_t value)
{
    lcd1602a_interface_send_data(LCD1602A_INTERFACE_MODE_DATA, value);
}
