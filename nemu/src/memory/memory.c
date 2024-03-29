#include "nemu.h"
#include "device/mmio.h"
#include "memory/mmu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define PTE_ADDR(pte) ((uint32_t)(pte) & ~0xfff)
#define PDX(va) (((uint32_t)(va) >> 22) & 0x3ff)
#define PTX(va) (((uint32_t)(va) >> 12) & 0x3ff)
#define OFF(va) ((uint32_t)(va)&0xfff)

#define pmem_rw(addr, type) *(type *)({                                       \
  Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
  guest_to_host(addr);                                                        \
})

uint8_t pmem[PMEM_SIZE];
static paddr_t page_translate(vaddr_t addr, bool is_write);

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len)
{
  int r = is_mmio(addr);
  if (r == -1)
  {
    return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  }
  else
  {
    return mmio_read(addr, len, r);
  }
}

void paddr_write(paddr_t addr, int len, uint32_t data)
{
  int r = is_mmio(addr);
  if (r == -1)
  {
    memcpy(guest_to_host(addr), &data, len);
  }
  else
  {
    mmio_write(addr, len, data, r);
  }
}

uint32_t vaddr_read(vaddr_t addr, int len)
{
  if ((((addr) + (len)-1) & ~PAGE_MASK) != ((addr) & ~PAGE_MASK))
  {
    uint32_t data = 0;
    for (int i = 0; i < len; i++)
    {
      paddr_t paddr = page_translate(addr + i, false);
      data += (paddr_read(paddr, 1)) << 8 * i;
    }
    return data;
  }
  else
  {
    paddr_t paddr = page_translate(addr, false);
    return paddr_read(paddr, len);
  }
}

void vaddr_write(vaddr_t addr, int len, uint32_t data)
{
  if ((((addr) + (len)-1) & ~PAGE_MASK) != ((addr) & ~PAGE_MASK))
  {
    for (int i = 0; i < len; i++)
    {
      paddr_t paddr = page_translate(addr + i, true);
      paddr_write(paddr, 1, data >> 8 * i);
    }
  }
  else
  {
    paddr_t paddr = page_translate(addr, true);
    paddr_write(paddr, len, data);
  }
}

static paddr_t page_translate(vaddr_t addr, bool is_write)
{
  paddr_t dir = (addr >> 22) & 0x3ff;
  paddr_t page = (addr >> 12) & 0x3ff;
  paddr_t offset = addr & 0xfff;
  if (cpu.cr0.paging)
  {
    uint32_t pdb = cpu.cr3.page_directory_base;
    uint32_t pt = paddr_read((pdb << 12) + (dir << 2), 4);
    assert(pt & 1);

    uint32_t pf = paddr_read((pt & 0xfffff000) + (page << 2), 4);
    if (!(pf & 1))
    {
      printf("%x\n", cpu.eip);
    }
    assert(pf & 1);

    return (pf & 0xfffff000) + offset;
  }
  return addr;
}