
#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>







int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    if (!fs || !pathname || pathname[0] != '/') return -1;

    int inumber = ROOT_INUMBER;
    if (strcmp(pathname, "/") == 0) return inumber;

    char path[1024];
    strncpy(path, pathname, sizeof(path));
    path[sizeof(path) - 1] = '\0';

    char *token = strtok(path + 1, "/");
    while (token != NULL) {
        struct direntv6 dirEnt;
        if (directory_findname(fs, token, inumber, &dirEnt) < 0) return -1;
        inumber = dirEnt.d_inumber;
        token = strtok(NULL, "/");
    }

    return inumber;
}
