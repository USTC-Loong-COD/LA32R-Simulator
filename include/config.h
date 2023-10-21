#ifndef __CONFIG_H__
#define __CONFIG_H__

#define CONFIG_PMEM_SIZE 0x08000000 // 128 MB
#define CONFIG_PMEM_BASE 0x1c000000


#ifndef CONFIG_REF
#define ITRACE
#define DEVICE
#endif

#define PAGE_SHIFT        12
#define PAGE_SIZE         (1ul << PAGE_SHIFT)
#define PAGE_MASK         (PAGE_SIZE - 1)

#endif // __CONFIG_H__