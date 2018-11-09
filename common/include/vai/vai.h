#ifndef _LIBVAI_H_
#define _LIBVAI_H_

#define CL_SIZE 64
#define CL(x) CL_SIZE*(x)

#define VAI_PAGE_SIZE 4096
#define VAI_PAGE_SHIFT 12
#define VAI_IS_PAGE_ALIGNED(x) (!((x)&((1<<VAI_PAGE_SHIFT)-1)))

#define VAI_MAGIC 0xBB
#define VAI_BASE 0x0

typedef struct {
    uint8_t data[16];
} afu_id_t;

typedef enum {
    VAI_AFU_HW,
    VAI_AFU_SIM
} afu_type_t;

struct vai_afu_conn {
    afu_type_t type;
    afu_id_t afu_id;

    union {
        struct {
            int fd;
            char *desc;
        } hw;
        struct {
            int payload;
        } sim;
    };
};

struct vai_afu_conn *vai_afu_connect(void);
int vai_afu_disconnect(struct vai_afu_conn *conn);
int vai_afu_alloc_region(struct vai_afu_conn *conn, void **buf_addr,
            uint64_t prefered_addr, uint64_t length);
int vai_afu_free_region(struct vai_afu_conn *conn, void *buf_addr);

#endif
