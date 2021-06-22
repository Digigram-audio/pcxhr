
snd-pcxhr-objs := pcxhr.o pcxhr_hwdep.o pcxhr_mixer.o pcxhr_core.o pcxhr_mix22.o pcxhr_hrtimer.o
# pcxhr_clock_monitoring.o 
obj-$(CONFIG_SND_PCXHR) += snd-pcxhr.o

# KVERSION is the kernel version
# If not defined, it is set to the current kernel version
KVERSION ?= $(shell uname -r)

# KDIR is the location of the kernel source.  The current standard is
# to link to the associated source tree from the directory containing
# the compiled modules.
KDIR  := /lib/modules/$(KVERSION)/build

# PWD is the current working directory and the location of our module
# source files.
PWD   := $(shell pwd)

# Set CFLAGS according to the needs
#Activate sound logs
#CFLAGS_MODULE += -DCONFIG_SND_DEBUG_VERBOSE

MODULES_DIR := /lib/modules/$(shell uname -r)/digigram
# default is the default make target.  The rule here says to run make
# with a working directory of the directory containing the kernel
# source and compile only the modules in the PWD (local) directory.
default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

all: clean default install-firmwares

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	
install-firmwares:
	test ! -d /lib/firmware/pcxhr/firmware || rm -Rf /lib/firmware/pcxhr/firmware
	test -d /lib/firmware/pcxhr || mkdir /lib/firmware/pcxhr/
	cp -Rf ./firmware/* /lib/firmware/pcxhr/

update-firmware:
	test ! -d /lib/firmware/pcxhr || mkdir /lib/firmware/pcxhr/
	cp ./firmware/dspe924e5.e56 /lib/firmware/pcxhr/

install:
	test -d $(MODULES_DIR) || mkdir $(MODULES_DIR)
	cp *.ko /lib/modules/$(shell uname -r)/digigram
	depmod -ae

	
.PHONY: default all clean install-firmwares update-firmware install
