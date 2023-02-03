#!/bin/sh

echo "Inserting bluetooth-tools..."
modprobe hci_uart
hciattach /dev/ttyS0 bcm43xx 921600 noflow
sdptool add SP

/etc/init.d/blue.sh >/dev/null 2>&1 &


echo "Inserting Led Device Driver..."
insmod led.ko

mkdir /dev/mqueue
mount -t mqueue none /dev/mqueue

# # daemon process
# /root/dSensors.elf

# # main process
# /root/d3.elf