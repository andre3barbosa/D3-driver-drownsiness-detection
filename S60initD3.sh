#!/bin/sh

echo "Inserting bluetooth-tools..."
modprobe hci_uart
hciattach /dev/ttyS0 bcm43xx 921600 noflow
sdptool add SP

echo "Inserting Led Device Driver..."
insmod /root/led.ko

# daemon process
/root/dSensors.elf &

# main process
/root/d3.elf