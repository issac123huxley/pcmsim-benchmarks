#!/bin/sh

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 
   exit 1
fi

mount -t tmpfs -o size=256m tmpfs_bench /mnt

echo "tmpfs created at /mnt"

exit 0