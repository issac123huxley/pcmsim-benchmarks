#!/bin/sh

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 
   exit 1
fi

modprobe brd rd_nr=1 rd_size=268435456 max_part=1

echo "tmpfs created at /mnt"

exit 0