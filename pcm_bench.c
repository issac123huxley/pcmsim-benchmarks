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

/**
 * 64  MB : 4 * 16  -> N 16
 * 128 MB : 4 * 32  -> N 32
 * 256 MB : 4 * 64  -> N 64
 * 512 MB : 4 * 128 -> N 128
 * 
 */
#define N 16

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
	unsigned long long disk_size   = 0;
	__time_t	   tv_sec_res  = 0;
	__syscall_slong_t  tv_nsec_res = 0;

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
	printf("Size of pcm in bytes: %llu, or %.3f MB\n", disk_size,
	       (double)disk_size / (1024 * 1024));

	addr = mmap(NULL, disk_size, PROT_READ | PROT_WRITE,
		    MAP_SHARED | MAP_SYNC, fd, 0);

	close(fd);

	clock_gettime(CLOCK_REALTIME, &start_time_pcm);

	for (int i = 0; i < (N * 1024 * 1024); i++)
		addr[i] = buf_src[i];

	clock_gettime(CLOCK_REALTIME, &end_time_pcm);

	// timings

	printf("start_time_pcm.tv_sec   : %ld\n"
	       "start_time_pcm.tv_nsec  : %ld\n",
	       start_time_pcm.tv_sec, start_time_pcm.tv_nsec);

	printf("end_time_pcm.tv_sec     : %ld\n"
	       "end_time_pcm.tv_nsec    : %ld\n",
	       end_time_pcm.tv_sec, end_time_pcm.tv_nsec);

	tv_sec_res = end_time_pcm.tv_sec - start_time_pcm.tv_sec;
	if (start_time_pcm.tv_nsec > end_time_pcm.tv_nsec) {
		tv_sec_res--;
		tv_nsec_res = (1000000000 - start_time_pcm.tv_nsec) +
			      end_time_pcm.tv_nsec;
	} else
		tv_nsec_res = end_time_pcm.tv_nsec - start_time_pcm.tv_nsec;

	printf("DDR to PCM of %d MB     : %ld,%ld sec\n", N * sizeof(int),
	       tv_sec_res, tv_nsec_res);

	free(buf_src);
	munmap(addr, disk_size);

	return EXIT_SUCCESS;
}