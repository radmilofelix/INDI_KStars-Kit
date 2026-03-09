#!/bin/bash

###########################################################################

#    Reset USB subsystems and retrigger udev rules 

###########################################################################

usb31only()
{
# USB 3.1 Only
    for port in $(lspci | grep xHCI | cut -d' ' -f1); do
	echo -n "0000:${port}"| sudo tee /sys/bus/pci/drivers/xhci_hcd/unbind;
	sleep 5;
	echo -n "0000:${port}" | sudo tee /sys/bus/pci/drivers/xhci_hcd/bind;
	sleep 5;
    done
    echo "usb 3.1 only finished."

}

usball()
{
# All USB
    echo "Resetting all USB subsystems, wait until it's done..."
    for port in $(lspci | grep USB | cut -d' ' -f1); do
	echo -n "0000:${port}"| sudo tee /sys/bus/pci/drivers/xhci_hcd/unbind;
	sleep 5;
	echo -n "0000:${port}" | sudo tee /sys/bus/pci/drivers/xhci_hcd/bind;
	sleep 5;
    done
    echo
    echo "USB susbsystems reset finished."
}


retriggerudev()
{
    echo "Reapplying udev rules..."
    udevadm trigger
    echo "done."
    echo
}


all()
{
    usball
    retriggerudev
    systemctl restart mountAtBoot
}

help()
{
    echo
    echo "Usage: `basename "$0"` <usb31only | usball | all | retriggerudev | help>"
    echo "usb31only - reset USB 3.1 subsystems"
    echo "usball - reset all USB subsystems"
    echo "all - reset all USB subsystems and retrigger udev rules"
    echo "retriggerudev - retrigger udev rules"
    echo "help - display this help message"
    echo
    echo
}

if [ -z $1 ]
then
    help
fi

$1
