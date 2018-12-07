#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <uuid/uuid.h>
#include <signal.h>

#include <vai/vai.h>
#include "csr_addr.h"

struct vai_afu_conn *conn;

struct status_cl {
    uint64_t completion;
    uint64_t n_clk;
    uint32_t n_read;
    uint32_t n_write;
};

void handler(int sig) {
    printf("disconnecting and exiting...\n");
    vai_afu_disconnect(conn);
    exit(-1);
}

int vai_memcpy_test(void)
{
    char *src[4], *dst[4];
    struct status_cl *stat[4];
    int i, j;
    uint64_t id_lo, id_hi;
    uint64_t nafus;

    /* read mux uuid */
    vai_afu_mmio_read(conn, 0x8, &id_lo);
    vai_afu_mmio_read(conn, 0x10, &id_hi);
    printf("%llx%llx\n", id_lo, id_hi);

    vai_afu_mmio_read(conn, 0x20, &nafus);
    printf("total AFUs: %llu\n", nafus);

    vai_afu_mmio_write(conn, 0x18, 0xffffffff);
    vai_afu_mmio_write(conn, 0x18, 0x0);

    /* setup buffers */
    for (i=0; i<3; i++) {
        vai_afu_alloc_region(conn, (void *)&(src[i]), 0, 4096*21);
        dst[i] = src[i] + 40960;
        stat[i] = (struct status_cl *) (dst[i] + 40960);
        printf("src[%d]: %llx, dst[%d]: %llx, stat[%d]: %llx\n",
                    i, (uint64_t)src[i], i, (uint64_t)dst[i], i, (uint64_t)stat[i]);

        for (j=0; j<40960; j++) {
            src[i][j] = rand()%256;
            dst[i][j] = 0;
        }

        stat[i]->completion = 0;
        stat[i]->n_clk = 0;
        stat[i]->n_read = 0;
        stat[i]->n_write = 0;
    }

    /* setup offset */
    vai_afu_mmio_write(conn, 0x30, (0-(uint64_t)src[0]/CL(1)));
    vai_afu_mmio_write(conn, 0x38, ((uint64_t)64*1024*1024*1024/CL(1)-(uint64_t)src[1]/CL(1)));
    vai_afu_mmio_write(conn, 0x40, ((uint64_t)128*1024*1024*1024/CL(1)-(uint64_t)src[2]/CL(1)));
    vai_afu_mmio_write(conn, 0x48, ((uint64_t)192*1024*1024*1024/CL(1)-(uint64_t)src[3]/CL(1)));

#define MMIO_VMOFF(x) (0x100*(x)+0x100)

    /* setup registers */

    for (i=0; i<3; i++) {
        /* read afu uuid */
        vai_afu_mmio_read(conn, MMIO_VMOFF(i)+0x8, &id_lo);
        vai_afu_mmio_read(conn, MMIO_VMOFF(i)+0x10, &id_hi);
        printf("%llx%llx\n", id_lo, id_hi);

        vai_afu_mmio_write(conn, MMIO_VMOFF(i)+MMIO_CSR_SOFT_RST, 0L);
        vai_afu_mmio_write(conn, MMIO_VMOFF(i)+MMIO_CSR_STATUS_ADDR, (uint64_t)stat[i]/CL(1));
        vai_afu_mmio_write(conn, MMIO_VMOFF(i)+MMIO_CSR_SRC_ADDR, (uint64_t)src[i]/CL(1));
        vai_afu_mmio_write(conn, MMIO_VMOFF(i)+MMIO_CSR_DST_ADDR, (uint64_t)dst[i]/CL(1));
        vai_afu_mmio_write(conn, MMIO_VMOFF(i)+MMIO_CSR_NUM_LINES, 64/CL(1));
        vai_afu_mmio_write(conn, MMIO_VMOFF(i)+MMIO_CSR_WR_THRESHOLD, 6);

        printf("[%d] start!\n", i);
    }

    for (i=0; i<3; i++) {
        vai_afu_mmio_write(conn, MMIO_VMOFF(i)+MMIO_CSR_CTL, 1);
    }


    while (0 == stat[0]->completion || 0 == stat[1]->completion ||
                0 == stat[2]->completion) {
        usleep(500000);
    }

    for (i=0; i<3; ++i) {
        for (j=0; j<64; ++j) {
            if (src[i][j] != dst[i][j])
                goto error;
        }
    }

    printf("everything is ok!\n");

    for (i=0; i<3; i++) {
        vai_afu_free_region(conn, (void*)src[i]);
    }

    return 0;

error:
    printf("[%d] wrong at %zu, get %u instead of %u\n", i, j, dst[j], src[j]);
    for (i=0; i<3; i++) {
        vai_afu_free_region(conn, (void*)src[i]);
    }
    return -1;
}

int main(int argc, char *argv[])
{
    void *buf_addr;
    volatile char *buf;
    uint64_t id_lo, id_hi;
    time_t t;

    srand((unsigned) time(&t));

    conn = vai_afu_connect();
    signal(SIGINT, handler);

    vai_memcpy_test();

    vai_afu_disconnect(conn);

    return 0;
}
