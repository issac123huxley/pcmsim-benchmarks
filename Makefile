CC=gcc
ARMCC=/home/luisky/PCMSIM_STAGE/kernelbuild/buildroot-2019.02.2/output/host/usr/bin/arm-linux-gnueabihf-gcc

.PHONY: amd64_all
amd64_all: pcm_bench ddr_bench

pcm_bench: pcm_bench.c
	$(CC) -o pcm_bench pcm_bench.c

ddr_bench: ddr_bench.c
	$(CC) -o ddr_bench ddr_bench.c

.PHONY: arm_all
arm_all: pcm_bench_arm ddr_bench_arm export_arm

pcm_bench_arm: pcm_bench.c
	$(ARMCC) -o pcm_bench_arm pcm_bench.c

ddr_bench_arm: ddr_bench.c
	$(ARMCC) -o ddr_bench_arm ddr_bench.c

export_arm:
	sudo ./export_arm.sh

.PHONY: clean
clean:
	rm pcm_bench ddr_bench pcm_bench_arm ddr_bench_arm