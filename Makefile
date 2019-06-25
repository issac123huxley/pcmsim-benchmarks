CC=gcc
ARMCC=/home/luisky/PCMSIM_STAGE/kernelbuild/buildroot-2019.02.2/output/host/usr/bin/arm-linux-gnueabihf-gcc

.PHONY: amd64
amd64: pcmsim_benchmark_mmap pcmsim_benchmark_io

pcmsim_benchmark_mmap: pcmsim_benchmark_mmap.c
	$(CC) -o pcmsim_benchmark_mmap pcmsim_benchmark_mmap.c

pcmsim_benchmark_io: pcmsim_benchmark_io.c
	$(CC) -o pcmsim_benchmark_io pcmsim_benchmark_io.c

.PHONY: arm
arm: pcmsim_benchmark_mmap_arm pcmsim_benchmark_io_arm export_arm

pcmsim_benchmark_mmap_arm: pcmsim_benchmark_mmap.c
	$(ARMCC) -o pcmsim_benchmark_mmap pcmsim_benchmark_mmap.c

pcmsim_benchmark_io_arm: pcmsim_benchmark_io.c
	$(ARMCC) -o pcmsim_benchmark_io pcmsim_benchmark_io.c

export_arm:
	sudo ./export_arm.sh

.PHONY: clean
clean:
	$(RM) pcmsim_benchmark_mmap pcmsim_benchmark_io
		 