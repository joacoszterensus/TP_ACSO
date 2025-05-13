#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"



int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    struct inode in;
    if (fs == NULL || buf == NULL) return -1;
    if (inumber < 1) return -1;

    if (inode_iget(fs, inumber, &in) < 0) return -1;
    if (!(in.i_mode & IALLOC)) return -1;

    int logicalBlock = inode_indexlookup(fs, &in, blockNum);
    if (logicalBlock <= 0) return 0;

    if (logicalBlock == -1) return -1;

    int size = inode_getsize(&in);
    int totalBlocks = (size + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;

    if (blockNum >= totalBlocks) return 0;

    int bytesRead = diskimg_readsector(fs->dfd, logicalBlock, buf);
    if (bytesRead < 0) return -1;

    int remainingBytes = size - blockNum * DISKIMG_SECTOR_SIZE;
    if (remainingBytes > DISKIMG_SECTOR_SIZE) remainingBytes = DISKIMG_SECTOR_SIZE;

    return remainingBytes;
}


