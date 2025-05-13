#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"




int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    if (inumber < 1 || inumber >= fs->superblock.s_isize * 16) {
        return -1;
    }

    int INODES_PER_SECTOR = DISKIMG_SECTOR_SIZE / sizeof(struct inode);

    int inodeSector = INODE_START_SECTOR + (inumber - 1) / INODES_PER_SECTOR;
    int offset = (inumber - 1) % INODES_PER_SECTOR;

    struct inode buffer[INODES_PER_SECTOR];
    int result = diskimg_readsector(fs->dfd, inodeSector, buffer);
    if (result == -1) return -1;

    *inp = buffer[offset];
    return 0;
}




int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp, int blockNum) {
    
    if (blockNum < 0) return -1;
    if (fs == NULL || inp == NULL) return -1;

    if (!(inp->i_mode & ILARG)) {
        if (blockNum >= 8) return -1;
        return inp->i_addr[blockNum];
    }

    if (blockNum < 256 * 7) {
        int indirectBlock = inp->i_addr[blockNum / 256];
        if (indirectBlock == 0) return -1;

        uint16_t block[256];
        if (diskimg_readsector(fs->dfd, indirectBlock, block) == -1) return -1;

        return block[blockNum % 256];
    }

    // Double-indirect
    int remaining = blockNum - (256 * 7);
    if (remaining >= 256 * 256) return -1;

    int doublyIndirectBlock = inp->i_addr[7];
    if (doublyIndirectBlock == 0) return -1;

    uint16_t indirects[256];
    if (diskimg_readsector(fs->dfd, doublyIndirectBlock, indirects) == -1) return -1;

    int firstIndex = remaining / 256;
    int secondIndex = remaining % 256;

    int indirectBlock = indirects[firstIndex];
    if (indirectBlock == 0) return -1;

    uint16_t final[256];
    if (diskimg_readsector(fs->dfd, indirectBlock, final) == -1) return -1;

    return final[secondIndex];
}


int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}
