#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#define BUF_SIZE_MB 64
#define BUF_SIZE BUF_SIZE_MB * 1024 * 1024
#define NB_LOOP 10

#define DDR_STR "DDR"
#define PCM_STR "PCM"
#define PCM_MODE_WRITE 'W'
#define PCM_MODE_MMAP 'M'

#define handle_error(msg)                                                      \
	do {                                                                   \
		perror(msg);                                                   \
		exit(EXIT_FAILURE);                                            \
	} while (0)

unsigned long long disk_size = 0;

void bench_init(const char *mem_type, const char pcm_mode, int *fd,
		char **addr);
void bench_write(int fd, void *src, size_t len);
void bench_memcpy(void *dest, void *src, size_t len, const char *mem_type);
void print_timings(struct timespec *start_time, struct timespec *end_time,
		   const char *mem_type);
void bench_exit(const char *mem_type, const char pcm_mode, int *fd,
		char **addr);

int main(int argc, char *argv[])
{
	if (argc < 2 || argc > 3) {
		printf("usage: %s mem_type [mode]\nmem_types are %s and %s\n"
		       "modes are %c for write and %c for mmap\n",
		       argv[0], DDR_STR, PCM_STR, PCM_MODE_WRITE,
		       PCM_MODE_MMAP);
		exit(EXIT_FAILURE);
	}

	int   fd;
	int   buf_size = BUF_SIZE_MB * 1024 * 1024;
	char *buf_src  = NULL;
	char *addr     = NULL;

	const char *mem_type = argv[1];
	const char  pcm_mode = (argc == 3) ? argv[2] : '\0';

	buf_src = malloc(buf_size);
	if (!buf_src)
		handle_error("malloc");

	memset(buf_src, 0, buf_size);

	bench_init(mem_type, pcm_mode, &fd, &addr);

	for (int i = 0; i < NB_LOOP; i++) {
		printf("Iteration number %d : \n", i);
		if (pcm_mode == PCM_MODE_WRITE)
			bench_write(fd, buf_src, buf_size);
		else
			bench_memcpy(addr, buf_src, buf_size, mem_type);
	}

	bench_exit(mem_type, pcm_mode, &fd, &addr);
	free(buf_src);

	return EXIT_SUCCESS;
}

void bench_init(const char *mode, const char pcm_mode, int *fd, char **addr)
{
	/* 
	// In order not to use cache:
	// must be root to do that, so either sudo or su
	system("echo 3 > /proc/sys/vm/drop_caches");
	// In order to sync data to persistent memory
	//(maybe only useful for pcm_bench)
	system("sync");*/

	if (!strcmp(mode, DDR_STR)) {
		*addr = malloc(BUF_SIZE);

		if (!(*addr))
			exit(EXIT_FAILURE);

	} else if (!strcmp(mode, PCM_STR)) {
		*fd = open("/dev/pcm0", O_RDWR | O_SYNC, 0777);

		if (*fd == -1)
			handle_error("open");

		ioctl(*fd, BLKGETSIZE64, &disk_size);
		printf("Size of pcm in bytes: %llu, or %.3f MB\n", disk_size,
		       (double)disk_size / (1024 * 1024));

		if (!strcmp(mode, PCM_MODE_MMAP)) {
			*addr = mmap(NULL, disk_size, PROT_READ | PROT_WRITE,
				     MAP_SHARED | MAP_SYNC, *fd, 0);
			if (!(*addr))
				handle_error("mmap");
		}

	} else {
		printf("no such memtype exists\nmmem_types are %s and %s\n",
		       DDR_STR, PCM_STR);
	}
}

void bench_write(int fd, void *src, size_t len)
{
	struct timespec start_time, end_time;

	clock_gettime(CLOCK_REALTIME, &start_time);

	write(fd, src, len);

	clock_gettime(CLOCK_REALTIME, &end_time);

	print_timings(&start_time, &end_time, PCM_STR);
}

void bench_memcpy(void *dest, void *src, size_t len, const char *mem_type)
{
	struct timespec start_time, end_time;

	clock_gettime(CLOCK_REALTIME, &start_time);

	memcpy(dest, src, len);

	clock_gettime(CLOCK_REALTIME, &end_time);

	print_timings(&start_time, &end_time, mem_type);
}

void print_timings(struct timespec *start_time, struct timespec *end_time,
		   const char *mem_type)
{
	__time_t	  tv_sec_res  = 0;
	__syscall_slong_t tv_nsec_res = 0;

	printf("start_time_pcm.tv_sec   : %ld\n"
	       "start_time_pcm.tv_nsec  : %ld\n",
	       start_time->tv_sec, start_time->tv_nsec);

	printf("end_time_pcm.tv_sec     : %ld\n"
	       "end_time_pcm.tv_nsec    : %ld\n",
	       end_time->tv_sec, end_time->tv_nsec);

	tv_sec_res = end_time->tv_sec - start_time->tv_sec;
	if (start_time->tv_nsec > end_time->tv_nsec) {
		tv_sec_res--;
		tv_nsec_res =
			(1000000000 - start_time->tv_nsec) + end_time->tv_nsec;
	} else
		tv_nsec_res = end_time->tv_nsec - start_time->tv_nsec;

	printf("DDR to %s of %d MB sec : %ld\n"
	       "DDR to %s of %d MB ns  : %ld\n",
	       mem_type, BUF_SIZE_MB, tv_sec_res, mem_type, BUF_SIZE_MB,
	       tv_nsec_res);
}

void bench_exit(const char *mode, const char pcm_mode, int *fd, char **addr)
{
	if (!strcmp(mode, DDR_STR)) {
		free(*addr);
	} else {
		if (pcm_mode == PCM_MODE_MMAP)
			munmap(addr, disk_size);
		close(*fd);
	}
}