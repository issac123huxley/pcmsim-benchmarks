#!/bin/sh

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 
   exit 1
fi

modprobe brd rd_nr=1 rd_size=131072 max_part=1

echo "ramdisk created at /dev/ram0"

exit 0