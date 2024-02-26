## UID: 005944382

(IMPORTANT: Only replace the above numbers with your true UID, do not modify spacing and newlines, otherwise your tarfile might not be created correctly)

# A Kernel Seedling

This kernel module creates a file /proc/count which displays the current number of running processes and deletes the file when the module is unloaded.

## Building

To build, a Makefile is required. Create a file called `Makefile` in your lab0 directory. Then paste the following code.
```
README_FILE=README.md
UID = $(shell cat ${README_FILE} | grep '\#\# UID'| grep -oe '\([0-9.]*\)')
SUBMISSION_FILES = proc_count.c ${README_FILE}

ifneq ($(KERNELRELEASE),)
obj-m := proc_count.o
else
KDIR ?= /lib/modules/`uname -r`/build

default:
	$(MAKE) -C $(KDIR) M=$$PWD modules

modules_install:
	$(MAKE) -C $(KDIR) M=$$PWD modules_install

install:
	$(MAKE) -C $(KDIR) M=$$PWD install

clean:
	$(MAKE) -C $(KDIR) M=$$PWD clean
	rm -f ${UID}-lab0-submission.tar
endif

tar:
	@tar -cf ${UID}-lab0-submission.tar ${SUBMISSION_FILES}
```
Now, type the following in the terminal to build the kernel module.
```
make
```

## Running

To run the kernel module, run the following command in the terminal.
```
sudo insmod proc_count.ko
```
To view the result, you can `cat /proc/file` to view the integer number of running processes.

## Cleaning Up

To remove the kernel module, enter the following code in the terminal.
```
sudo rmmod proc_count
```

## Testing

The kernel release version is `5.14.8-arch1-1`.
