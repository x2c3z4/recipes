ifneq ($(KERNELRELEASE),)
	obj-m:=addr.o
else
	KDIR:=/lib/modules/$(shell uname -r)/build
	PWD:=$(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

endif
