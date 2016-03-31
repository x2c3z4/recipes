obj-m:=pagehash.o
pagehash-objs := tcrypt.o testmgr.o 
KDIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
.PHONEY:uninstall

install: uninstall
	sudo insmod pagehash.ko
uninstall:
	sudo rmmod pagehash.ko
