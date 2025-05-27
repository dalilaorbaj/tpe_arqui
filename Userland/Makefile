include Makefile.inc

SHELL_DATA=0001-shellDataModule.bin

all: ShellCodeModule ShellDataModule

ShellCodeModule:
	cd ShellCodeModule; make

ShellDataModule:
	printf "This is shell data." >> $(SHELL_DATA) && dd if=/dev/zero bs=1 count=1 >> $(SHELL_DATA)

clean:
	cd ShellCodeModule; make clean
	rm -rf *.bin


.PHONY: ShellCodeModule all clean
