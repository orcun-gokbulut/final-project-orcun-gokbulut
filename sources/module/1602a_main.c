#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

int driver_1602a_init_module(void);
void driver_1602a_deinit_module(void);

int driver_1602a_init_module(void)
{
    printk(KERN_INFO "1602A LCD Driver Initialized.\n");

    return 0;
}

void driver_1602a_deinit_module(void)
{
    printk(KERN_INFO "1602A LCD Driver Cleaned Up.\n");
}

module_init(driver_1602a_init_module);
module_exit(driver_1602a_deinit_module);

MODULE_AUTHOR("Yigit Orcun Gokbulut"); /** TODO: fill in your name **/
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("1602A LCD Driver");
MODULE_VERSION("1.0");

