nasm -I ./boot/ -o ./boot/boot.bin ./boot/boot.s
nasm -I ./boot/ -o ./boot/loader.bin ./boot/loader.s
dd if=./boot/boot.bin of=hard_disk.img bs=512 count=1 conv=notrunc
dd if=./boot/loader.bin of=hard_disk.img bs=512 count=4 seek=2 conv=notrunc
dd if=./hx_kernel of=hard_disk.img bs=512 count=200 seek=9 conv=notrunc

