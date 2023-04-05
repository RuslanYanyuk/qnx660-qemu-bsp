# qnx660-qemu-bsp

## Prerequisites
* Debian 11
* QEMU emulator version 5.2.0 for ARM
* FTP server

## How to boot

1. Install `u-boot-qemu` package
2. Configure anonymous FTP server to point to the directory with built images (.ifs files)
3. Run emulator
```bash
qemu-system-arm -M virt -m 2g  -cpu cortex-a15 -bios /usr/lib/u-boot/qemu_arm/u-boot.bin -monitor telnet:127.0.0.1:6299,server,nowait -serial mon:stdio
```
4. Enter inside the u-boot shell
```
setenv serverip 192.168.0.5 && tftpboot 0x40400000 arm_qnx.ifs &&  go 0x40400000
```
replace `192.168.0.5` with the address of your network adapter

Now you should be able to promt the QNX shell

Enjoy!
