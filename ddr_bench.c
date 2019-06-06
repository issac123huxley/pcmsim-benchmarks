#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

/**
 * 64  MB : 4 * 16  -> N 16
 * 128 MB : 4 * 32  -> N 32
 * 256 MB : 4 * 64  -> N 64
 * 512 MB : 4 * 128 -> N 128
 * 
 */
#define N 16

int main(int argc, char **argv)
{
	struct timespec   start_time_ddr, end_time_ddr;
	__time_t	  tv_sec_res  = 0;
	__syscall_slong_t tv_nsec_res = 0;

	// sizeof(int) = 4;
	// the stack is only 8Mb, me need malloc for such huge buffers

	// In order not to use cache:
	system("echo 3 > /proc/sys/vm/drop_caches");
	// In order to sync data to persistent memory
	//(maybe only useful for ddr_bench)
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

	// timings

	printf("start_time_ddr.tv_sec   : %ld\n"
	       "start_time_ddr.tv_nsec  : %ld\n",
	       start_time_ddr.tv_sec, start_time_ddr.tv_nsec);

	printf("end_time_ddr.tv_sec     : %ld\n"
	       "end_time_ddr.tv_nsec    : %ld\n",
	       end_time_ddr.tv_sec, end_time_ddr.tv_nsec);

	tv_sec_res = end_time_ddr.tv_sec - start_time_ddr.tv_sec;
	if (start_time_ddr.tv_nsec > end_time_ddr.tv_nsec) {
		tv_sec_res--;
		tv_nsec_res = (1000000000 - start_time_ddr.tv_nsec) +
			      end_time_ddr.tv_nsec;
	} else
		tv_nsec_res = end_time_ddr.tv_nsec - start_time_ddr.tv_nsec;

	printf("DDR to DDR of %d MB     : %ld,%ld sec\n", N * sizeof(int),
	       tv_sec_res, tv_nsec_res);

	free(buf_src);
	free(buf_cpy);

	return EXIT_SUCCESS;
}