#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
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

// uncommentent this to print results to stdout
//#define PRINT_MSG

#define BUF_SIZE_MB 64
#define BUF_SIZE BUF_SIZE_MB * 1024 * 1024
#define DEF_NB_LOOP 10

#define DDR_STR "DDR"
#define PCM_STR "PCM"

#define handle_error(msg)                                                      \
	do {                                                                   \
		perror(msg);                                                   \
		exit(EXIT_FAILURE);                                            \
	} while (0)

unsigned long long disk_size   = 0;
FILE *		   result_file = NULL;

void bench_init(const char *mem_type, int *fd);
void bench_write(int fd, void *src, size_t len);
void bench_read(int fd, void *dest, size_t len);
void print_timings(struct timespec *start_time, struct timespec *end_time);
void bench_exit(const char *mem_type, int fd);
inline void drop_cache(void);

int main(int argc, char *argv[])
{
	if (argc < 4 || argc > 5) {
		printf("usage: %s mem_type [buf_size] [nb_loop]\n"
		       "mem_types are %s and %s, buf_size in MB\n",
		       argv[0], DDR_STR, PCM_STR);
		exit(EXIT_FAILURE);
	}

	int   fd;
	char *buf_src = NULL;
	char *endptr;

	//TODO: check errno for strtol
	const char *mem_type = argv[1];
	const int   buf_size = (argc >= 3) ?
				     (strtol(argv[2], &endptr, 10) * 1048576) :
				     BUF_SIZE_MB * 1048576;
	const int nb_loop =
		(argc >= 4) ? strtol(argv[3], &endptr, 10) : DEF_NB_LOOP;

	buf_src = malloc(buf_size);
	if (!buf_src)
		handle_error("malloc");

	memset(buf_src, 65, buf_size);

	bench_init(mem_type, &fd);

	fprintf(result_file, "MEMCPY, %d\n\ntime_microseconds\n", buf_size);
	for (int i = 0; i < nb_loop; i++) {
#ifdef PRINT_MSG
		printf("Iteration number %d : \n", i);
#endif
		bench_write(fd, buf_src, buf_size);
		lseek(fd, 0, SEEK_SET);
	}

	fprintf(result_file, "\n\nMEMCPY, %d\n\ntime_microseconds\n", buf_size);
	for (int i = 0; i < nb_loop; i++) {
#ifdef PRINT_MSG
		printf("Iteration number %d : \n", i);
#endif
		bench_read(fd, buf_src, buf_size);
		lseek(fd, 0, SEEK_SET);
	}

	bench_exit(mem_type, fd);
	free(buf_src);

	return EXIT_SUCCESS;
}

void bench_init(const char *mem_type, int *fd)
{
	result_file = fopen(mem_type, "w+");

	if (!strcmp(mem_type, DDR_STR)) {
		*fd = open("/dev/ram0", O_RDWR | O_SYNC, 0777);

		puts("************");
		puts("DDR benchark");
		puts("************");

	} else if (!strcmp(mem_type, PCM_STR)) {
		*fd = open("/dev/pcm0", O_RDWR | O_SYNC, 0777);

		puts("*************");
		puts("PCM benchmark");
		puts("*************");

	} else {
		printf("no such memtype\nmmem_types are %s and %s\n", DDR_STR,
		       PCM_STR);
		exit(EXIT_FAILURE);
	}

	if (*fd == -1)
		handle_error("open");

	ioctl(*fd, BLKGETSIZE64, &disk_size);
	printf("Size of disk in bytes: %llu, or %.3f MB\n", disk_size,
	       (double)disk_size / (1024 * 1024));
}

void bench_write(int fd, void *src, size_t len)
{
	struct timespec start_time, end_time;

	drop_cache();
	clock_gettime(CLOCK_REALTIME, &start_time);
	write(fd, src, len);
	clock_gettime(CLOCK_REALTIME, &end_time);

#ifdef PRINT_MSG
	puts("MEMCPY:");
#endif

	print_timings(&start_time, &end_time);
}

void bench_read(int fd, void *dest, size_t len)
{
	struct timespec start_time, end_time;

	drop_cache();
	clock_gettime(CLOCK_REALTIME, &start_time);
	read(fd, dest, len);
	clock_gettime(CLOCK_REALTIME, &end_time);

	for (int i = 0; i < 10; i++) {
		printf("value %d : %c\n", i, ((char *)dest)[i]);
	}

#ifdef PRINT_MSG
	puts("MEMREAD:");
#endif

	print_timings(&start_time, &end_time);
}

void print_timings(struct timespec *start_time, struct timespec *end_time)
{
	__time_t	  tv_sec_res  = 0;
	__syscall_slong_t tv_nsec_res = 0;

	tv_sec_res = end_time->tv_sec - start_time->tv_sec;
	if (start_time->tv_nsec > end_time->tv_nsec) {
		tv_sec_res--;
		tv_nsec_res =
			(1000000000 - start_time->tv_nsec) + end_time->tv_nsec;
	} else
		tv_nsec_res = end_time->tv_nsec - start_time->tv_nsec;

	fprintf(result_file, "%ld\n",
		((tv_sec_res * 1000000) + (tv_nsec_res / 1000)));

#ifdef PRINT_MSG
	printf("Time sec : %ld\n"
	       "Time ns  : %ld\t%d us\t%d ms\n",
	       tv_sec_res, tv_nsec_res, (tv_nsec_res / 1000),
	       (tv_nsec_res / 1000000));
#endif
}

void bench_exit(const char *mode, int fd)
{
	fclose(result_file);
	close(fd);

	puts("Done. Results in DDR or PCM file.");
}

void drop_cache(void)
{
	// In order to sync data to persistent memory
	system("sync");
	// In order not to use cache:
	// must be root to do that, so either sudo or su
	system("echo 3 > /proc/sys/vm/drop_caches");
}