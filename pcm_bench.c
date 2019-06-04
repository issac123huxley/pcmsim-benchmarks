#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/fs.h>
#include <sys/ioctl.h>

#define N 64 // 256MB : 4 * 64, 512 MB : 4 * 128

#define handle_error(msg)                                                      \
	do {                                                                   \
		perror(msg);                                                   \
		exit(EXIT_FAILURE);                                            \
	} while (0)

int main(int argc, char *argv[])
{
	int *		   addr; // equivalent to buf_cpy in ddr_bench
	int		   fd;
	struct timespec    start_time_pcm, end_time_pcm;
	unsigned long long disk_size = 0;

	// In order not to use cache:
	// must be root to do that, so either sudo or su
	system("echo 3 > /proc/sys/vm/drop_caches");
	// In order to sync data to persistent memory
	//(maybe only useful for pcm_bench)
	system("sync");

	int *buf_src = malloc(sizeof(int) * N * 1024 * 1024);
	if (buf_src == NULL)
		handle_error("malloc");

	for (int i = 0; i < (N * 1024 * 1024); i++)
		buf_src[i] = i;

	fd = open("/dev/pcm0", O_RDWR, 0777);
	if (fd == -1)
		handle_error("open");

	ioctl(fd, BLKGETSIZE64, &disk_size);
	printf("Number of bytes: %llu, this makes %.3f MB\n", disk_size,
	       (double)disk_size / (1024 * 1024));

	addr = mmap(NULL, disk_size, PROT_READ | PROT_WRITE,
		    MAP_SHARED | MAP_SYNC, fd, 0);

	close(fd);

	clock_gettime(CLOCK_REALTIME, &start_time_pcm);

	for (int i = 0; i < (N * 1024 * 1024); i++)
		addr[i] = buf_src[i];

	clock_gettime(CLOCK_REALTIME, &end_time_pcm);

	printf("start_time_drr : %ld\n",
	       ((start_time_pcm.tv_sec * 1000000000) + start_time_pcm.tv_nsec));

	printf("end_time_drr.tv_nsec: %ld\n",
	       ((end_time_pcm.tv_sec * 1000000000) + end_time_pcm.tv_nsec));

	printf("Time taken to copy %ldMb of data from one point in ddr to"
	       "another: %ld ns\n",
	       N * sizeof(int),
	       ((end_time_pcm.tv_sec * 1000000000) + end_time_pcm.tv_nsec) -
		       ((start_time_pcm.tv_sec * 1000000000) +
			start_time_pcm.tv_nsec));

	free(buf_src);
	munmap(addr, disk_size);

	return EXIT_SUCCESS;
}