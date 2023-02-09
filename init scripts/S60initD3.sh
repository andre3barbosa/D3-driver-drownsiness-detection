#!/bin/sh

echo "Inserting bluetooth-tools..."
modprobe hci_uart
hciattach /dev/ttyS0 bcm43xx 921600 noflow
sdptool add SP

/etc/init.d/blue.sh >/dev/null 2>&1 &

mkdir /dev/mqueue
mount -t mqueue none /dev/mqueue

# Daemon process
/root/dSensors.elf

# Main process
/root/d3.elf