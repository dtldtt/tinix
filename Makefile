#!Makefile
#
# --------------------------------------------------------
#
#    hurlex 这个小内核的 Makefile
#    默认使用的C语言编译器是 GCC、汇编语言编译器是 nasm
#
# --------------------------------------------------------
#

# patsubst 处理所有在 C_SOURCES 字列中的字（一列文件名），如果它的 结尾是 '.c'，就用 '.o' 把 '.c' 取代
C_SOURCES = $(shell find . -name "*.c")
C_OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES))
S_SOURCES = $(shell find . -name "*.s")
S_OBJECTS = $(patsubst %.s, %.o, $(S_SOURCES))

CC = gcc
LD = ld
ASM = nasm

C_FLAGS = -c -Wall -m32 -ggdb -gstabs+ -nostdinc -fno-builtin -fno-stack-protector -I include
LD_FLAGS = -T scripts/kernel.ld -m elf_i386 -nostdlib
ASM_FLAGS = -f elf -g -F stabs -I boot/

all: $(S_OBJECTS) $(C_OBJECTS) link load_image

# The automatic variable `$<' is just the first prerequisite
.c.o:
	@echo 编译代码文件 $< ...
	$(CC) $(C_FLAGS) $< -o $@

.s.o:
	@echo 编译汇编文件 $< ...
	$(ASM) $(ASM_FLAGS) $<

link:
	@echo 链接内核文件...
	$(LD) $(LD_FLAGS) $(S_OBJECTS) $(C_OBJECTS) -o hx_kernel

.PHONY:clean
clean:
	$(RM) $(S_OBJECTS) $(C_OBJECTS) hx_kernel ./boot/*.bin

.PHONY:load_image
load_image:
	nasm -I ./boot/ -o ./boot/boot.bin ./boot/boot.s
	nasm -I ./boot/ -o ./boot/loader.bin ./boot/loader.s
	dd if=./boot/boot.bin of=hard_disk.img bs=512 count=1 conv=notrunc
	dd if=./boot/loader.bin of=hard_disk.img bs=512 count=4 seek=2 conv=notrunc
	dd if=./hx_kernel of=hard_disk.img bs=512 count=200 seek=9 conv=notrunc

.PHONY:update_image
update_image:
	sudo mount hard_disk.img /mnt/kernel
	sudo cp hx_kernel /mnt/kernel/hx_kernel
	sleep 1
	sudo umount /mnt/kernel

.PHONY:mount_image
mount_image:
	sudo mount hard_disk.img /mnt/kernel

.PHONY:umount_image
umount_image:
	sudo umount /mnt/kernel

.PHONY:qemu
qemu:
	qemu -hda hard_disk.img -boot c
	#qemu -fda floppy.img -boot a	
	#add '-nographic' option if using server of linux distro, such as fedora-server,or "gtk initialization failed" error will occur.
	# -fda 使用文件作为软盘, -hda 使用文件作为硬盘  -boot 启动选项，默认从硬盘启动，a表示从软盘启动,c表示硬盘

.PHONY:bochs
bochs:
	bochs -f scripts/bochsrc.disk

.PHONY:debug
debug:
	qemu -S -s -hda hard_disk.img -boot c &
	#qemu -S -s -fda floppy.img -boot a &
	sleep 1
	cgdb -x scripts/gdbinit

