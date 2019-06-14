#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <linux/fs.h>
#include <sys/ioctl.h>

#define N 64 //MB

#define handle_error(msg)                                                      \
	do {                                                                   \
		perror(msg);                                                   \
		exit(EXIT_FAILURE);                                            \
	} while (0)

void bench_write(int fd, void *src, size_t len);

int main(int argc, char *argv[])
{
	int		   fd;
	unsigned long long disk_size = 0;

	// In order not to use cache:
	// must be root to do that, so either sudo or su
	system("echo 3 > /proc/sys/vm/drop_caches");
	// In order to sync data to persistent memory
	//(maybe only useful for pcm_bench)
	system("sync");

	int   buf_size = N * 1024 * 1024;
	char *buf_src  = malloc(buf_size);
	if (buf_src == NULL)
		handle_error("malloc");

	memset(buf_src, 0, buf_size);

	fd = open("/dev/pcm0", O_RDWR | O_SYNC, 0777);
	if (fd == -1)
		handle_error("open");

	ioctl(fd, BLKGETSIZE64, &disk_size);
	printf("Size of pcm in bytes: %llu, or %.3f MB\n", disk_size,
	       (double)disk_size / (1024 * 1024));

	puts("first write");
	bench_write(fd, buf_src, buf_size);

	puts("second write");
	bench_write(fd, buf_src, buf_size);

	puts("third write");
	bench_write(fd, buf_src, buf_size);

	close(fd);
	free(buf_src);

	return EXIT_SUCCESS;
}

void bench_write(int fd, void *src, size_t len)
{
	struct timespec   start_time_pcm, end_time_pcm;
	__time_t	  tv_sec_res  = 0;
	__syscall_slong_t tv_nsec_res = 0;

	clock_gettime(CLOCK_REALTIME, &start_time_pcm);

	write(fd, src, len);

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

	printf("DDR to PCM of %d MB sec : %ld\n"
	       "DDR to PCM of %d MB ns  : %ld\n",
	       N, tv_sec_res, N, tv_nsec_res);
}