
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSCULL_DEBUG
else
  DEBFLAGS = -O2
endif

EXTRA_CFLAGS += $(DEBFLAGS)

ifneq ($(KERNELRELEASE),)

obj-m += lcd1602a.o
lcd1602a-y := lcd1602a_main.o lcd1602a_interface.o lcd1602a_device.o bcm2711_gpio_registers.o bitwise_ops.o

else

KERNEL_PATH ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

modules:
	$(MAKE) -C $(KERNEL_PATH) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNEL_PATH) M=$(PWD) modules_install

clean:
	$(MAKE) -C $(KERNEL_PATH) M=$(PWD) clean

PHONY:load
load: modules
	sudo $(PWD)/lcd1602a.sh start

.PHONY: unload
unload:
	sudo $(PWD)/lcd1602a.sh stop

endif