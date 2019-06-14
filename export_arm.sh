#!/bin/sh

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 
   exit 1
fi

rm /run/media/luisky/rootfs/root/pcmsim_benchmark
cp pcmsim_benchmark /run/media/luisky/rootfs/root/pcmsim_benchmark

if [ $? = 1 ]
then
        echo "error: is SD card plugged in ?"
        exit 1
fi

echo "ddr_bench_arm & pcm_bench_arm updated on SD card"

exit 0