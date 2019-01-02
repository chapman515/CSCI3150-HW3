#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_BUF_SIZE 128
const char* HD = "HD";
unsigned char BUF[MAX_BUF_SIZE];

int main(int argc, char **args)
{
	if( argc != 3 )
	{
		printf("\n Usage: ./hd_checker offset numBytes\n");
		printf("\t offset: number, the offset (in bytes) of the chunck you want to read on the virtual disk\n");
		printf("\t numBytes: number, the size (in bytes) of the chunck you want to read on the vitural disk\n");
		printf("\n the result is displayed in hexadecimal format, each line contains a word (4 bytes)\n");
		return -1;
	}
	int offset = atoi(args[1]);
	int numBytes = atoi(args[2]);
	if( numBytes > MAX_BUF_SIZE)
	{
		printf("argument numBytes should be less than 128\n");
		return -1;
	}
	int fd = open(HD, O_RDONLY);
	if(fd < 0)
	{
		printf("Error in open()!\n");
		return -1;
	}
	int currpos = lseek(fd, offset, SEEK_SET);
	//printf("%d\n", currpos);
	if(currpos < 0)
	{
		printf("Error in lseek()\n");
		return -1;
	}
	int bytesCnt = read(fd,(void *)BUF, numBytes);
	//printf("%d\n", bytesCnt);
	printf("the data from address %d to %d on virtual hard disk:\n", offset, offset+numBytes);
	for(int i = 0; i < bytesCnt; i++)
	{
		if(i%4 == 0) printf("\n");
		printf("%02x ", BUF[i]); 
	}
	printf("\n");
	close(fd);
	return 0;
}
