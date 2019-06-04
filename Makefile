CC=gcc
ARMCC=/path/to/arm/cc

.PHONY: all
all: ddr_bench pcm_bench

pcm_bench: pcm_bench.c
	$(CC) -o pcm_bench pcm_bench.c

ddr_bench: ddr_bench.c
	$(CC) -o ddr_bench ddr_bench.c

.PHONY: arm_all
arm_all: ddr_bench_arm pcm_bench_arm

ddr_bench_arm:
	$(ARMCC) -o ddr_bench_arm pcm_bench.c

pcm_bench_arm:
	$(ARMCC) -o pcm_bench_arm ddr_bench.c

.PHONY: clean
clean:
	rm pcm_bench ddr_bench ddr_bench_arm pcm_bench_arm