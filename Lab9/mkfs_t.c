#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include "superblock.h"
#include "inode.h"

int write_sb(int fd)
{
        superblock *sb=(superblock *)malloc(sizeof(superblock));
	if(sb == NULL){
		printf("Error: malloc()\n");
		return -1;
	}
	
	sb->inode_offset=INODE_OFFSET;
	sb->data_offset=DATA_OFFSET;
	sb->max_inode=MAX_INODE;
	sb->max_data_blk=MAX_DATA_BLK;
	sb->next_available_inode=1;
	sb->next_available_blk=1;
	sb->blk_size=BLOCK_SIZE;

	int currpos=lseek(fd, SB_OFFSET, SEEK_SET);
	if(currpos<0){
		printf("Error: lseek()\n");
		return -1;
	}

	//write superblock
	int ret = write(fd, (void *)sb, sizeof(superblock));
	if(ret != sizeof (superblock) ){
		printf("Error: write()\n");
		return -1;
	}

	fsync(fd);

	return 0;	
}

int  inode_rootdir(int fd)
{
        //write inode 0		
	inode * ip=(inode *)malloc(sizeof(inode));
	if(ip == NULL){
               	printf("Error: malloc()\n");
		return -1;
        }

        ip->i_number=0;
	ip->i_mtime=time(NULL);
	ip->i_type=1;
	ip->i_size=sizeof(DIR_NODE)*2;
	ip->i_blocks=1;
	ip->direct_blk[0]=0;
	ip->file_num=2;

	int currpos = lseek(fd,INODE_OFFSET, SEEK_SET);
	if(currpos<0){
		printf("Error: lseek()\n");
		return -1;
	}
	int ret = write(fd, (void *)ip, sizeof(inode));
	if(ret != sizeof (inode) ){
		printf("Error: write()\n");
		return -1;
	}

	fsync(fd);
	return 0;		
}
		
int init_rootdir(int fd)
{
        DIR_NODE * first=(DIR_NODE *)malloc (sizeof(DIR_NODE));
        strcpy(first->dir,".");
        first->inode_number=0;

        DIR_NODE *second=(DIR_NODE *)malloc(sizeof(DIR_NODE));
        strcpy(second->dir,"..");
        second->inode_number=0;

        int currpos = lseek(fd, DATA_OFFSET, SEEK_SET);
	if(currpos<0){
		printf("Error: lseek()\n");
		return -1;
	}

        int ret = write(fd, (void *)first, sizeof( DIR_NODE));
	if(ret != sizeof (DIR_NODE) ){
		printf("Error: write()\n");
		return -1;
	}

        ret = write(fd, (void *)second, sizeof( DIR_NODE));
	if(ret != sizeof (DIR_NODE) ){
		printf("Error: write()\n");
		return -1;
	}

        fsync(fd);

	return 0;
}

int main(int argc, char **args)
{
	//Usage: ./mkfs HD
	if( argc != 2 ){
		printf("Usage: ./mkfs_t file_name\n");
		return -1;
	}

	//Open the file
	int fd = open (args[1], O_WRONLY);
	if(fd<0){       
		printf("Error: open()\n");
		return -1;
	}

	//Write the superblock data structure
	if ( write_sb(fd) < 0 ){
		printf("Error: write_sb()\n");
		return -1;
	}

	//Create inode 0 for the root directory
	if( inode_rootdir(fd) < 0 ){
		printf("Error: inode_rootdir()\n");
		return -1;
	}

	//Write . and .. in the root directory (Block 0)
	if( init_rootdir(fd) < 0 ){
		printf("Error: init_rootdir()\n");
		return -1;
	}

	close(fd);

	return 0;
}
