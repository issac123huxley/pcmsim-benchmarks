CC=gcc
ARMCC=/home/luisky/PCMSIM_STAGE/kernelbuild/buildroot-2019.02.2/output/host/usr/bin/arm-linux-gnueabihf-gcc

.PHONY: amd64
amd64: pcmsim_benchmark_mmap pcmsim_benchmark_write

pcmsim_benchmark_mmap: pcmsim_benchmark_mmap.c
	$(CC) -o pcmsim_benchmark_mmap pcmsim_benchmark_mmap.c

pcmsim_benchmark_write: pcmsim_benchmark_write.c
	$(CC) -o pcmsim_benchmark_write pcmsim_benchmark_write.c

.PHONY: arm
arm: pcmsim_benchmark_mmap_arm pcmsim_benchmark_write_arm export_arm

pcmsim_benchmark_mmap_arm: pcmsim_benchmark_mmap.c
	$(ARMCC) -o pcmsim_benchmark_mmap pcmsim_benchmark_mmap.c

pcmsim_benchmark_write_arm: pcmsim_benchmark_write.c
	$(ARMCC) -o pcmsim_benchmark_write pcmsim_benchmark_write.c

export_arm:
	sudo ./export_arm.sh

.PHONY: clean
clean:
	$(RM) pcmsim_benchmark_mmap pcmsim_benchmark_write
		 