#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

#define N 64 //MB

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

	int   buf_size = N * 1024 * 1024;
	char *buf_src  = malloc(buf_size);
	if (buf_src == NULL)
		exit(EXIT_FAILURE);

	char *buf_cpy = malloc(buf_size);
	if (buf_cpy == NULL)
		exit(EXIT_FAILURE);

	memset(buf_src, 0, buf_size);

	clock_gettime(CLOCK_REALTIME, &start_time_ddr);

	for (int i = 0; i < buf_size; i++)
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

	printf("DDR to DDR of %d MB sec : %ld\n"
	       "DDR to DDR of %d MB ns  : %ld\n",
	       N, tv_sec_res, N, tv_nsec_res);

	free(buf_src);
	free(buf_cpy);

	return EXIT_SUCCESS;
}