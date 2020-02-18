#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>

//content in fs.h
#define ROOTINO 1  // root i-number
#define BSIZE 512  // block size

// File system super block
struct superblock {
  uint size;         // Size of file system image (blocks)
  uint nblocks;      // Number of data blocks
  uint ninodes;      // Number of inodes.
};

#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))
#define MAXFILE (NDIRECT + NINDIRECT)

// On-disk inode structure
struct dinode {
  short type;           // File type
  short major;          // Major device number (T_DEV only)
  short minor;          // Minor device number (T_DEV only)
  short nlink;          // Number of links to inode in file system
  uint size;            // Size of file (bytes)
  uint addrs[NDIRECT+1];   // Data block addresses
};

// Inodes per block.
#define IPB           (BSIZE / sizeof(struct dinode))

// Block containing inode i
#define IBLOCK(i)     ((i) / IPB + 2)

// Bitmap bits per block
#define BPB           (BSIZE*8)

// Block containing bit for block b
#define BBLOCK(b, ninodes) (b/BPB + (ninodes)/IPB + 3)

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

struct dirent {
  ushort inum;
  char name[DIRSIZ];
};

//end of fs.h


int main(int argc, char *argv[]) {
	if (argc != 2 ) {
		fprintf(stderr, "Usage: xcheck <file_system_image>\n");
		exit(1);
	}
	int image = open(argv[1], O_RDONLY);
	if (image == -1) {
		fprintf(stderr, "image not found.\n");
		exit(1);
	}
	//get the file size
	struct stat sbuf;
	fstat(image, &sbuf);
	size_t length = sbuf.st_size;
	//map the file to memory
	char *addr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, image, 0);
	
	//super block struct
	struct superblock *sb;
	sb = (struct superblock*)(addr + BSIZE);
	
	//inode 
	struct dinode *dip = (struct dinode *)(addr + 2*BSIZE);	
//	void *bitmap = (void*)((sb->ninodes/IPB + 3)*BSIZE);
	//list of used blocks
//	int usedBlocks[sb->size];
	//list of used links
//	int numLinks[sb->ninodes];
	//list of used inodes
//	int inodeUsed[sb->ninodes];
	//directory list of used inodes
//	int usedinodeDirectory[sb->ninodes];
	
//	for (int i = 0; i < sb->size; i ++ )	
//		usedBlocks[i] = 0;
	//make used blockes to used
//	for (int i = 0; i < (sb->ninodes/IPB + 3 + (sb->size/(BSIZE*8)+1)); i++)
//		usedBlocks[i] = 1;
	

	//traverse each inode	
	for (int i = 0; i < sb->ninodes; i++) {
		short type = dip->type;
		
		//check3: Root directory exists
		if (i == 0 && type != 1) {
			fprintf(stderr, "ERROR: root directory does not exist.\n");
			exit(1);
		}
		
		//check1: Each inode is either unallocated or one of the valid types (T_FILE, T_DIR, T_DEV)
		if (!(type > 0 && type <= 3))	{
			fprintf(stderr, "ERROR: bad inode.\n");
			exit(1);
		}
		else {
			//inode is not unallocated
			if (type!= 0) {
				//inode is a directory, T_DIR
				if (type == 1) {
					void *blkAddr = addr + dip->addrs[0] * BSIZE;
					struct dirent *currDirectory = (struct dirent*)(blkAddr);
					//check4: . entry in this directory
					if (strcmp(currDirectory->name, ".") != 0) {
						fprintf(stderr, "ERROR: directory not properly formatted.\n");
						exit(1);
					}
					
					currDirectory++;
					//check4: .. entries
					if (strcmp(currDirectory->name, "..") != 0) {
						fprintf(stderr, "ERROR: directory not properly formatted.\n");
                                                exit(1);
                                        }
							
				
				}
			}	
		}
	}

	return 0;
}								
