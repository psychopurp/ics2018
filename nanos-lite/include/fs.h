#ifndef __FS_H__
#define __FS_H__

#include "common.h"

enum
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

int fs_open(const char *filename, int flags, int mode);
ssize_t fs_read(int fd, void *buf, size_t len);
int fs_close(int fd);
size_t fs_filesz(int fd);

#endif
