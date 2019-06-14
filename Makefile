CC=gcc
ARMCC=/home/luisky/PCMSIM_STAGE/kernelbuild/buildroot-2019.02.2/output/host/usr/bin/arm-linux-gnueabihf-gcc

.PHONY: amd64
amd64: pcmsim_benchmark

pcmsim_benchmark_amd64: pcmsim_benchmark.c
	$(CC) -o pcmsim_benchmark pcmsim_benchmark.c

.PHONY: arm
arm: pcmsim_benchmark_arm export_arm

pcmsim_benchmark_arm: pcmsim_benchmark.c
	$(ARMCC) -o pcmsim_benchmark pcmsim_benchmark.c

export_arm:
	sudo ./export_arm.sh

.PHONY: clean
clean:
	$(RM) pcmsim_benchmark
		 