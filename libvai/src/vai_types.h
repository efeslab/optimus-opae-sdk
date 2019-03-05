#ifndef _VAI_TYPES_H_
#define _VAI_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <inttypes.h>
#endif

#define CL_SIZE 64
#define CL(x) CL_SIZE*(x)

#define VAI_PAGE_SIZE 4096
#define VAI_PAGE_SHIFT 12
#define VAI_IS_PAGE_ALIGNED(x) (!((x)&((1<<VAI_PAGE_SHIFT)-1)))

#define VAI_USER_TASK_ENTRY_SIZE CL(3)

#define VAI_MAGIC 0xBB
#define VAI_BASE 0x0

/**
 * VAI_GET_AFU_VERSION
 *
 * Report the version of the driver API by entering the field in the given struct.
 * Return: 0 on success, -errno on failure.
 */
#define VAI_GET_AFU_ID _IO(VAI_MAGIC, VAI_BASE + 0)


struct vai_map_info {
    uint64_t user_addr;
    uint64_t length;
};

/**
 * VAI_DMA_MAP_REGION
 *
 * Map a DMA region based on the provided struct vai_map_info.
 * Return: 0 on success, -errno on failure.
 */
#define VAI_DMA_MAP_REGION _IO(VAI_MAGIC, VAI_BASE + 1)

/**
 * VAI_DMA_UNMAP_REGION
 *
 * Unmap a DMA region based on the provided struct vai_map_info.
 * Return: 0 on success, -errno on failure.
 */
#define VAI_DMA_UNMAP_REGION _IO(VAI_MAGIC, VAI_BASE + 2)

struct vai_user_task_entry {
    uint8_t private[VAI_USER_TASK_ENTRY_SIZE];
};

/**
 * VAI_TASK_SUBMIT
 *
 * Submit a task to the virtualized hardware based on the provided struct vai_task_entry.
 * Return: 0 on success, -errno on failure.
 */
#define VAI_SUBMIT_TASK _IO(VAI_MAGIC, VAI_BASE + 3)

/**
 * VAI_PULL_TASK
 *
 * Pull the ring to get finished tasks.
 * Return: the number of finished tasks.
 */
#define VAI_PULL_TASK _IO(VAI_MAGIC, VAI_BASE + 4)

/**
 * VAI_SET_MEM_BASE
 *
 * Set the start of memory address space allocator will use. It is finally used
 * by hardware multiplexer.
 * Return: 0 on success, -errno on failure.
 */
#define VAI_SET_MEM_BASE _IO(VAI_MAGIC, VAI_BASE + 5)

/**
 * VAI_RESET
 *
 * Set reset signal to the correspond afu.
 * Return: 0 on success, -errno on failure.
 */
#define VAI_SET_RESET _IO(VAI_MAGIC, VAI_BASE + 6)


#define VAI_ACCELERATOR_L 0x0
#define VAI_ACCELERATOR_H 0x8
#define VAI_PAGING_NOTIFY_MAP_ADDR 0x0
#define VAI_PAGING_NOTIFY_MAP 0x8
#define VAI_MEM_BASE 0x10
#define VAI_RESET 0x18

#define VAI_NOTIFY_DO_MAP 0x0
#define VAI_NOTIFY_DO_UNMAP 0x1
#define VAI_RESET_ENABLE 0x1

struct vai_paging_notifier {
    uint64_t va;
    uint64_t pa;
};

#endif /* _VAI_TYPES_H_ */
