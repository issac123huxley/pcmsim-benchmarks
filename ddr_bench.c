#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

#define N 64 // 256MB : 4 * 64, 512 MB : 4 * 128

int main(int argc, char **argv)
{
	struct timespec start_time_ddr, end_time_ddr;

	// sizeof(int) = 4;
	// the stack is only 8Mb, me need malloc for such huge buffers

	// In order not to use cache:
	system("echo 3 > /proc/sys/vm/drop_caches");
	// In order to sync data to persistent memory
	//(maybe only useful for pcm_bench)
	system("sync");

	int *buf_src = malloc(sizeof(int) * N * 1024 * 1024);
	if (buf_src == NULL)
		exit(EXIT_FAILURE);

	int *buf_cpy = malloc(sizeof(int) * N * 1024 * 1024);
	if (buf_cpy == NULL)
		exit(EXIT_FAILURE);

	for (int i = 0; i < (N * 1024 * 1024); i++)
		buf_src[i] = i;

	clock_gettime(CLOCK_REALTIME, &start_time_ddr);

	for (int i = 0; i < (N * 1024 * 1024); i++)
		buf_cpy[i] = buf_src[i];

	clock_gettime(CLOCK_REALTIME, &end_time_ddr);

	printf("start_time_drr : %ld\n",
	       ((start_time_ddr.tv_sec * 1000000000) + start_time_ddr.tv_nsec));

	printf("end_time_drr.tv_nsec: %ld\n",
	       ((end_time_ddr.tv_sec * 1000000000) + end_time_ddr.tv_nsec));

	printf("Time taken to copy %dMb of data from one point in ddr to"
	       "another: %ld ns\n",
	       N * sizeof(int),
	       ((end_time_ddr.tv_sec * 1000000000) + end_time_ddr.tv_nsec) -
		       ((start_time_ddr.tv_sec * 1000000000) +
			start_time_ddr.tv_nsec));

	free(buf_src);
	free(buf_cpy);

	return EXIT_SUCCESS;
}