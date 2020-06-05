
obj-m := ft245.o
#obj-m := rfsdr.o

KNAME   := $(shell uname -r)
#KNAME   := 4.7.5-200.fc24.x86_64
KDIR    := /lib/modules/$(KNAME)/build
PWD     := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	mkdir -p ../modules/$(KNAME)
	cp -f $(PWD)/ft245.ko ../modules/$(KNAME)/
	
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
