#include "lcd1602a_device.h"

#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/kthread.h>

#include "lcd1602a_interface.h"


#define LCD1602A_DEVICE_BUFFER_SIZE 256

static int lcd1602a_device_open(struct inode* inode, struct file* filp);
static long lcd1602a_device_ioctl(struct file* filp, unsigned int param, unsigned long value);
static ssize_t lcd1602a_device_write(struct file* filp, const char __user* buffer, size_t count, loff_t* offset);
static loff_t lcd1602a_device_llseek(struct file* filp, loff_t cursor, int set);
static int lcd1602a_device_release(struct inode* inode, struct file* filp);

static const struct file_operations lcd1602a_device_ops =
{
    .open = lcd1602a_device_open,
    .write = lcd1602a_device_write,
    .compat_ioctl = lcd1602a_device_ioctl,
    .llseek = lcd1602a_device_llseek,
    .release = lcd1602a_device_release
};

static struct
{
    struct cdev character_device;
    dev_t device_number;

    struct mutex buffer_mutex;
    char buffer[LCD1602A_DEVICE_BUFFER_SIZE];
    size_t buffer_size;

    struct task_struct* io_thread;
    char io_buffer[LCD1602A_DEVICE_BUFFER_SIZE];
    size_t io_buffer_size;
} lcd1602a_device = {};

static int lcd1602_device_io_thread(void* idx)
{
    while(!kthread_should_stop())
    {
        // Transfer Write Buffer to IO Buffer
        mutex_lock(&lcd1602a_device.buffer_mutex);
        if (lcd1602a_device.buffer_size == 0)
        {
            mutex_unlock(&lcd1602a_device.buffer_mutex);

            schedule();
            continue;
        }

        memcpy(lcd1602a_device.io_buffer, lcd1602a_device.buffer, lcd1602a_device.buffer_size);
        lcd1602a_device.io_buffer_size = lcd1602a_device.buffer_size;
        lcd1602a_device.buffer_size = 0;
        mutex_unlock(&lcd1602a_device.buffer_mutex);

        size_t i = 0;
        for (i = 0; i < lcd1602a_device.io_buffer_size; i++)
            lcd1602a_interface_send_data(LCD1602A_INTERFACE_MODE_DATA, lcd1602a_device.io_buffer[i]);

        lcd1602a_device.io_buffer_size = 0;
    }

    return 0;
}

static int lcd1602a_device_open(struct inode* inode, struct file* filp)
{
    if ((filp->f_flags & (O_RDWR | O_RDONLY)) != 0)
        return -EINVAL;

    return 0;
}

static long lcd1602a_device_ioctl(struct file* filp, unsigned int param, unsigned long value)
{
    return -EINVAL;
}

static ssize_t lcd1602a_device_write(struct file* filp, const char __user* buffer, size_t count, loff_t* offset)
{
    if (mutex_lock_interruptible(&lcd1602a_device.buffer_mutex) < 0)
        return -EINTR;

    if (lcd1602a_device.buffer_size == LCD1602A_DEVICE_BUFFER_SIZE)
    {
        mutex_unlock(&lcd1602a_device.buffer_mutex);
        return 0;
    }

    if (lcd1602a_device.buffer_size + count >= LCD1602A_DEVICE_BUFFER_SIZE)
        count -= LCD1602A_DEVICE_BUFFER_SIZE - lcd1602a_device.buffer_size;

    if (copy_from_user(lcd1602a_device.buffer + lcd1602a_device.buffer_size, buffer, count) != 0)
    {
        mutex_unlock(&lcd1602a_device.buffer_mutex);
        return -EFAULT;
    }

    lcd1602a_device.buffer_size += count;
    *offset += count;

    wake_up_process(lcd1602a_device.io_thread);

    mutex_unlock(&lcd1602a_device.buffer_mutex);

    return count;
}

static loff_t lcd1602a_device_llseek(struct file* filp, loff_t offset, int whence)
{
    return -EINVAL;
}

int lcd1602a_device_release(struct inode* inode, struct file* filp)
{
    return 0;
}

bool lcd1602a_device_initialize(void)
{
    int result = alloc_chrdev_region(&lcd1602a_device.device_number, 0, 1, "lcd1602a");
    if (result < 0)
        return false;

    cdev_init(&lcd1602a_device.character_device, &lcd1602a_device_ops);
    lcd1602a_device.character_device.owner = THIS_MODULE;
    lcd1602a_device.character_device.ops = &lcd1602a_device_ops;
    result = cdev_add(&lcd1602a_device.character_device, lcd1602a_device.device_number, 1);
    if (result < 0)
    {
        unregister_chrdev_region(lcd1602a_device.device_number, 1);
        return false;
    }

    mutex_init(&lcd1602a_device.buffer_mutex);

    lcd1602a_device.io_thread = kthread_create(&lcd1602_device_io_thread, 0, "klcd1602a_io/0");
    if (lcd1602a_device.io_thread == NULL)
    {
        mutex_destroy(&lcd1602a_device.buffer_mutex);
        cdev_del(&lcd1602a_device.character_device);
        unregister_chrdev_region(lcd1602a_device.device_number, 1);
    }

    wake_up_process(lcd1602a_device.io_thread);

    return true;
}

void lcd1602a_device_deinitialize(void)
{
    kthread_stop(lcd1602a_device.io_thread);
    mutex_destroy(&lcd1602a_device.buffer_mutex);
    cdev_del(&lcd1602a_device.character_device);
    unregister_chrdev_region(lcd1602a_device.device_number, 1);
    memset(&lcd1602a_device, 0, sizeof(lcd1602a_device));
}
