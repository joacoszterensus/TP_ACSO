#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>



int directory_findname(struct unixfilesystem *fs, const char *name,
  int dirinumber, struct direntv6 *dirEnt) {
if (!fs || !name || !dirEnt || dirinumber < 1) return -1;

struct inode in;
if (inode_iget(fs, dirinumber, &in) < 0) return -1;
if (!(in.i_mode & IALLOC) || (in.i_mode & IFMT) != IFDIR) return -1;

int size = inode_getsize(&in);
int numEntries = size / sizeof(struct direntv6);
char blockbuf[DISKIMG_SECTOR_SIZE];

for (int i = 0; i < numEntries; i++) {
int offset = i * sizeof(struct direntv6);
int blockNum = offset / DISKIMG_SECTOR_SIZE;
int blockOffset = offset % DISKIMG_SECTOR_SIZE;

int res = file_getblock(fs, dirinumber, blockNum, blockbuf);
if (res < 0 || blockOffset + sizeof(struct direntv6) > res) return -1;

struct direntv6 *entry = (struct direntv6 *)(blockbuf + blockOffset);
if (strncmp(entry->d_name, name, 14) == 0) {
*dirEnt = *entry;
return 0;
}
}

return -1;
}

