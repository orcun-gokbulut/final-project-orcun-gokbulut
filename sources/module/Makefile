
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DSCULL_DEBUG
else
  DEBFLAGS = -O2
endif

EXTRA_CFLAGS += $(DEBFLAGS)

ifneq ($(KERNELRELEASE),)

obj-m	:= 1602a.o
1602a-y := 1602a_main.o

else

KERNEL_PATH ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

modules:
	$(MAKE) -C $(KERNEL_PATH) M=$(PWD) modules

modules_install:
	$(MAKE) -C $(KERNEL_PATH) M=$(PWD) modules_install

endif

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c *.symvers *.order *.mod .tmp_versions
