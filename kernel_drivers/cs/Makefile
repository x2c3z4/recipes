target=$(TARGET)
obj-m:=$(target).o
KDIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
install:
	insmod $(target).ko 
rm:
	rmmod $(target).ko
