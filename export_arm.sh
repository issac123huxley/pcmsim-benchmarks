#!/bin/sh

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 
   exit 1
fi

rm /run/media/luisky/rootfs/root/ddr_bench_arm
rm /run/media/luisky/rootfs/root/pcm_bench_arm
cp ddr_bench_arm /run/media/luisky/rootfs/root/ddr_bench_arm
cp pcm_bench_arm /run/media/luisky/rootfs/root/pcm_bench_arm

if [ $? = 1 ]
then
        echo "error: is SD card plugged in ?"
        exit 1
fi

echo "ddr_bench_arm & pcm_bench_arm updated on SD card"

exit 0