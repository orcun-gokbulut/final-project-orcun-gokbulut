#include "lcd1602a_interface.h"

#include <linux/types.h>
#include <linux/io.h>
#include <linux/iomap.h>
#include <linux/delay.h>

#include "lcd1602a_bitwise_ops.h"

#define GPIO_BASE_ADDRESS (0xFE200000) // VNU 0x7E200000
#define GPIO_BASE_SIZE (GPIO_CLR_OFFSET + 4)
#define GPIO_FSEL_0_OFFSET (0x00)
#define GPIO_FSEL_1_OFFSET (0x04)
#define GPIO_FSEL_2_OFFSET (0x08)
#define GPIO_SET_OFFSET (0x1C)
#define GPIO_CLR_OFFSET (0x28)

#define GPIO_ADJUST_ADDRESS(base, offset) ((void __iomem*)((uint8_t __iomem*)(base) + (offset)))
#define GPIO_FSEL_0_ADDR(base) GPIO_ADJUST_ADDRESS(base, GPIO_FSEL_0_OFFSET)
#define GPIO_FSEL_1_ADDR(base) GPIO_ADJUST_ADDRESS(base, GPIO_FSEL_1_OFFSET)
#define GPIO_FSEL_2_ADDR(base) GPIO_ADJUST_ADDRESS(base, GPIO_FSEL_2_OFFSET)
#define GPIO_SET_ADDR(base) GPIO_ADJUST_ADDRESS((base), GPIO_SET_OFFSET) 
#define GPIO_CLR_ADDR(base) GPIO_ADJUST_ADDRESS((base), GPIO_CLR_OFFSET)

#define LCD1602A_INTERFACE_TAS 25 * 10
#define LCD1602A_INTERFACE_TPW 140 * 10
#define LCD1602A_INTERFACE_TAH 10 * 10

#define LCD1602A_IOMEM_REGION_LOCK 0

static const char *bit_rep[16] = {
    [ 0] = "0000", [ 1] = "0001", [ 2] = "0010", [ 3] = "0011",
    [ 4] = "0100", [ 5] = "0101", [ 6] = "0110", [ 7] = "0111",
    [ 8] = "1000", [ 9] = "1001", [10] = "1010", [11] = "1011",
    [12] = "1100", [13] = "1101", [14] = "1110", [15] = "1111",
};

static struct 
{
    void __iomem* register_base;
} lcd1602a_interface_data = {};

void lcd1602a_interface_zeroize(void)
{
    uint32_t temp = 0;
    LCD1602A_BIT_CLEAR(temp, 27); // RS
    LCD1602A_BIT_CLEAR(temp, 22); // RW
    LCD1602A_BIT_CLEAR(temp, 27); // E
    LCD1602A_BIT_CLEAR(temp, 5); // D0
    LCD1602A_BIT_CLEAR(temp, 6); // D1
    LCD1602A_BIT_CLEAR(temp, 13); // D2
    LCD1602A_BIT_CLEAR(temp, 26); // D3
    LCD1602A_BIT_CLEAR(temp, 23); // D4
    LCD1602A_BIT_CLEAR(temp, 24); // D5
    LCD1602A_BIT_CLEAR(temp, 25); // D6
    LCD1602A_BIT_CLEAR(temp, 16); // D7
    iowrite32(temp, GPIO_CLR_ADDR(lcd1602a_interface_data.register_base));
}

bool lcd1602a_interface_initialize(void)
{
    #if LCD1602A_IOMEM_REGION_LOCK
        if (request_mem_region(GPIO_BASE_ADDRESS, GPIO_BASE_SIZE, "bcm2835-gpiomem") == NULL)
        {
            printk(KERN_ERR "lcd1602a: Cannot lock the gpio registers memory region.");
            return false;
        }
    #endif

    lcd1602a_interface_data.register_base = ioremap(GPIO_BASE_ADDRESS, GPIO_BASE_SIZE);
    if (lcd1602a_interface_data.register_base == NULL)
    {
        printk(KERN_ERR "lcd1602a: Cannot map the gpio registers memory region.");
        return false; 
    }
    
    lcd1602a_interface_zeroize();

    uint32_t temp = ioread32(GPIO_FSEL_0_ADDR(lcd1602a_interface_data.register_base));
    LCD1602A_BITFIELD_SET(temp, 15, 3, 1); // D0:GPIO5
    LCD1602A_BITFIELD_SET(temp, 18, 3, 1); // D1:GPIO6
    iowrite32(temp, GPIO_FSEL_0_ADDR(lcd1602a_interface_data.register_base));

    temp = ioread32(GPIO_FSEL_1_ADDR(lcd1602a_interface_data.register_base));
    LCD1602A_BITFIELD_SET(temp, 9, 3, 1); // D2:GPIO13
    LCD1602A_BITFIELD_SET(temp, 18, 3, 1); // D7:GPIO16
    iowrite32(temp, GPIO_FSEL_1_ADDR(lcd1602a_interface_data.register_base));

    temp = ioread32(GPIO_FSEL_2_ADDR(lcd1602a_interface_data.register_base));
    LCD1602A_BITFIELD_SET(temp, 6, 3, 1); // RW:GPIO22
    LCD1602A_BITFIELD_SET(temp, 9, 3, 1); // D4:GPIO23
    LCD1602A_BITFIELD_SET(temp, 12, 3, 1); // D5:GPIO24
    LCD1602A_BITFIELD_SET(temp, 16, 3, 1); // D6:GPIO25
    LCD1602A_BITFIELD_SET(temp, 21, 3, 1); // E:GPIO27
    iowrite32(temp, GPIO_FSEL_2_ADDR(lcd1602a_interface_data.register_base));

    lcd1602a_interface_zeroize();

    return true;
}

void lcd1602a_interface_deinitialize(void)
{
    lcd1602a_interface_zeroize();

    iounmap(lcd1602a_interface_data.register_base);

    #if LCD1602A_IOMEM_REGION_LOCK
        release_mem_region(GPIO_BASE_ADDRESS, GPIO_BASE_SIZE);
    #endif
}

void lcd1602a_interface_send_data(lcd1602a_interface_mode_t rs, uint8_t data)
{
    printk(KERN_INFO "lcd1602a: Sending RS: %s Data: 0x%X (0b%s%s)", 
        (rs == LCD1602A_INTERFACE_MODE_COMMAND ? "Command" : "Data"),
        data,
        bit_rep[(data >> 4) & 0x0F],
        bit_rep[data & 0x0F]
    );

    uint32_t temp = 0;
    LCD1602A_BIT_SET_VALUE(temp, 27, rs); // RS
    LCD1602A_BIT_CLEAR(temp, 22); // RW
    LCD1602A_BIT_SET_VALUE(temp, 5, LCD1602A_BIT_GET(data, 0));
    LCD1602A_BIT_SET_VALUE(temp, 6, LCD1602A_BIT_GET(data, 1));
    LCD1602A_BIT_SET_VALUE(temp, 13, LCD1602A_BIT_GET(data, 2));
    LCD1602A_BIT_SET_VALUE(temp, 26, LCD1602A_BIT_GET(data, 3));
    LCD1602A_BIT_SET_VALUE(temp, 23, LCD1602A_BIT_GET(data, 4));
    LCD1602A_BIT_SET_VALUE(temp, 24, LCD1602A_BIT_GET(data, 5));
    LCD1602A_BIT_SET_VALUE(temp, 25, LCD1602A_BIT_GET(data, 6));
    LCD1602A_BIT_SET_VALUE(temp, 16, LCD1602A_BIT_GET(data, 7));
    iowrite32(~temp, GPIO_CLR_ADDR(lcd1602a_interface_data.register_base));
    iowrite32(temp, GPIO_SET_ADDR(lcd1602a_interface_data.register_base));
    ndelay(LCD1602A_INTERFACE_TAS);

    temp = 0;
    LCD1602A_BIT_SET(temp, 27); // E

    iowrite32(temp, GPIO_SET_ADDR(lcd1602a_interface_data.register_base));
    ndelay(LCD1602A_INTERFACE_TPW);

    iowrite32(temp, GPIO_CLR_ADDR(lcd1602a_interface_data.register_base));
    ndelay(LCD1602A_INTERFACE_TAH);
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
    LCD1602A_BIT_SET_VALUE(command, 1, increment);
    LCD1602A_BIT_SET_VALUE(command, 0, shift);
    lcd1602a_interface_send_data(LCD1602A_INTERFACE_MODE_COMMAND, command);
}

void lcd1602a_interface_set_display_control(bool display_on, bool cursor_on, bool cursor_position_on)
{
    uint8_t command = 0x04;
    LCD1602A_BIT_SET_VALUE(command, 2, display_on);
    LCD1602A_BIT_SET_VALUE(command, 1, cursor_on);
    LCD1602A_BIT_SET_VALUE(command, 0, cursor_position_on);
    lcd1602a_interface_send_data(LCD1602A_INTERFACE_MODE_COMMAND, command);
}

void lcd1602a_interface_set_cursor_display_shift(bool increment, bool direction)
{
    uint8_t command = 0x08;
    LCD1602A_BIT_SET_VALUE(command, 3, increment);
    LCD1602A_BIT_SET_VALUE(command, 2, direction);
    lcd1602a_interface_send_data(LCD1602A_INTERFACE_MODE_COMMAND, command);
}

void lcd1602a_interface_set_function_set(lcd1602a_data_length_t data_length, uint8_t number_of_lines, lcd1602a_font_size_t font_size)
{
    uint8_t command = 0x20;
    LCD1602A_BIT_SET_VALUE(command, 4, data_length);
    LCD1602A_BIT_SET_VALUE(command, 3, number_of_lines > 1);
    LCD1602A_BIT_SET_VALUE(command, 2, font_size);
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
