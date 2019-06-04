/*
#ifdef __x86_64__
int a = 0x10;
#endif

#ifdef __i386__
int a = 0x20;
#endif

#ifdef __arm__
int a = 0x30;
#endif

#ifdef __aarch64__
int a = 0x40;
#endif
*/

#include <fcntl.h>
#include <linux/fs.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


int main(int argc, char **argv)
{
	int fd;
	unsigned long long numblocks=0;

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) exit(EXIT_FAILURE);
	ioctl(fd, BLKGETSIZE64, &numblocks);
	close(fd);
	printf("Number of bytes: %llu, this makes %.3f GB\n",
		numblocks, 
		(double)numblocks / (1024 * 1024 * 1024));

	return EXIT_SUCCESS;
	//return a;
}
