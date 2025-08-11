#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include "lcd1602a_interface.h"
#include "lcd1602a_device.h"

static int lcd1602a_init_module(void);
static void lcd1602a_deinit_module(void);

static int __init lcd1602a_init_module(void)
{
    printk(KERN_INFO "lcd1602a: Initializing module...\n");
    
    if (!lcd1602a_interface_initialize())
    {
        printk(KERN_ERR "lcd1602a: Cannot initialize interface.");
        return -1;
    }

    if (!lcd1602a_device_initialize())
    {
        printk(KERN_ERR "lcd1602a: Cannot initialize character device.");
        return -1;
    }

    printk(KERN_INFO "lcd1602a: Module initialized.\n");

    return 0;
}

static void __exit lcd1602a_deinit_module(void)
{
    printk(KERN_INFO "lcd1602a: Deinitializing module...\n");

    lcd1602a_device_deinitialize();
    lcd1602a_interface_deinitialize();

    printk(KERN_INFO "lcd1602a: Module deinitialized.\n");
}

module_init(lcd1602a_init_module);
module_exit(lcd1602a_deinit_module);

MODULE_AUTHOR("Yigit Orcun Gokbulut"); /** TODO: fill in your name **/
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("1602A LCD Driver");
MODULE_VERSION("1.0");

