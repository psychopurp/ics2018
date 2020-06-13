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
  // // PDE pde, *pgdir;
  // // PTE pte, *pgtable;
  // // paddr_t paddr = addr;
  // CR0 cr0 = (CR0)cpu.cr0;
  // //保护模式+分页机制
  // if (cr0.protect_enable && cr0.paging)
  // {
  //   CR3 cr3 = (CR3)cpu.cr3;
  //   // 页目录表
  //   PDE *pgdirs = (PDE *)(PTE_ADDR(cr3.val));
  //   PDE pde = (PDE)paddr_read((uint32_t)(pgdirs + PDX(addr)), 4);
  //   Assert(pde.present, "addr=0x%x", addr);

  //   //二级页表
  //   PTE *ptab = (PTE *)(PTE_ADDR(pde.val));
  //   PTE pte = (PTE)paddr_read((uint32_t)(ptab + PTX(addr)), 4);
  //   Assert(pte.present, "addr=0x%x", addr);

  //   //设置accessed和dirty
  //   pte.accessed = 1;
  //   pde.accessed = 1;
  //   if (is_write)
  //     pte.dirty = 1;

  //   //物理地址
  //   paddr_t paddr = PTE_ADDR(pte.val) | OFF(addr);
  //   return paddr;
  //   // pgdir = (PDE *)PTE_ADDR(cpu.cr3.val);
  //   // pde.val = paddr_read((uint32_t)&pgdir[PDX(add_mmio_map)], 4);
  //   // assert(pde.present);
  //   // pde.accessed = 1;

  //   // pgtable = (PTE *)(PTE_ADDR(pde.val));
  //   // pte.val = paddr_read((uint32_t)&pgtable[PTX(addr)], 4);

  //   // assert(pte.present);
  //   // pte.accessed = 1;

  //   // pte.dirty = is_write ? 1 : pte.dirty;
  //   // paddr = PTE_ADDR(pte.val) | OFF(addr);
  // }
  // return addr;
}