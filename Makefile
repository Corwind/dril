KDIR ?= /lib/modules/$(shell uname -r)/build

obj-m := stressball.o

all: modules stressball_test

modules modules_install clean help:
	$(MAKE) -C $(KDIR) M=$(PWD) $@
