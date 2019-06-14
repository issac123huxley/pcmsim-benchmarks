CC=gcc
ARMCC=/home/luisky/PCMSIM_STAGE/kernelbuild/buildroot-2019.02.2/output/host/usr/bin/arm-linux-gnueabihf-gcc

.PHONY: amd64_all
amd64_all: pcm_write_bench pcm_mmap_bench ddr_bench

pcm_write_bench: pcm_write_bench.c
	$(CC) -o pcm_write_bench pcm_write_bench.c

pcm_mmap_bench: pcm_mmap_bench.c
	$(CC) -o pcm_mmap_bench pcm_mmap_bench.c

ddr_bench: ddr_bench.c
	$(CC) -o ddr_bench ddr_bench.c

.PHONY: arm_all
arm_all: pcm_write_bench_arm pcm_mmap_bench_arm ddr_bench_arm export_arm

pcm_write_bench_arm: pcm_write_bench.c
	$(CC) -o pcm_write_bench_arm pcm_write_bench.c

pcm_mmap_bench_arm: pcm_mmap_bench.c
	$(CC) -o pcm_mmap_bench_arm pcm_mmap_bench.c

ddr_bench_arm: ddr_bench.c
	$(ARMCC) -o ddr_bench_arm ddr_bench.c

export_arm:
	sudo ./export_arm.sh

.PHONY: clean
clean:
	rm 	pcm_write_bench pcm_mmap_bench ddr_bench \
		pcm_write_bench_arm pcm_mmap_bench_arm ddr_bench_arm \
		 