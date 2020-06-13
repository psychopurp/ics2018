#include "common.h"
#include "fs.h"
#include "memory.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;
#define RAMDISK_SIZE ((&ramdisk_end) - (&ramdisk_start))
extern void ramdisk_read(void *buf, off_t offset, size_t len);

uintptr_t loader(_Protect *as, const char *filename)
{
  // TODO();
  // ramdisk_read(DEFAULT_ENTRY, 0, RAMDISK_SIZE);
  int fd = fs_open(filename, 0, 0);
  Log("filename=%s,fd=%d", filename, fd);
  void *pa, *va = DEFAULT_ENTRY;
  int size = fs_filesz(fd);
  while (size > 0)
  {
    pa = new_page();
    _map(as, va, pa);
    fs_read(fd, pa, PGSIZE);
    va += PGSIZE;
    size -= PGSIZE;
  }
  fs_close(fd);
  // fs_read(fd, DEFAULT_ENTRY, fs_filesz(fd));
  // fs_close(fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
