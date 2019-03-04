#ifdef HAVE_CONFIG_H
#include <config.h>
#endif				// HAVE_CONFIG_H

#include "common_int.h"
#include <ase_common.h>

#include <errno.h>
#include <malloc.h>		/* malloc */
#include <stdlib.h>		/* exit */
#include <stdio.h>		/* printf */
#include <string.h>		/* memcpy */
#include <unistd.h>		/* getpid */
#include <sys/types.h>		/* pid_t */
#include <sys/ioctl.h>		/* ioctl */
#include <sys/mman.h>		/* mmap & munmap */
#include <sys/time.h>		/* struct timeval */

#include <vai/fpga.h>
#include "vai_internal.h"

struct vai_afu_conn *vai_afu_connect(void)
{
    struct vai_afu_conn *conn = (struct vai_afu_conn *)ase_malloc(sizeof(*conn));
	global_conn = conn;
    session_init();

    return conn;
}

fpga_result vai_afu_disconnect(struct vai_afu_conn *conn)
{
    session_deinit();
	global_conn = NULL;

    free(conn);

    return FPGA_OK;
}

fpga_result vai_afu_alloc_region(struct vai_afu_conn *conn, void **buf_addr,
            uint64_t prefered_addr, uint64_t length)
{
    struct buffer_t *buf;

    UNUSED_PARAM(conn);

    if ((prefered_addr != (uint64_t)-1L) &&
		(!(VAI_IS_PAGE_ALIGNED(prefered_addr)) || !(VAI_IS_PAGE_ALIGNED(length))))
        return FPGA_INVALID_PARAM;

    buf = (struct buffer_t *) ase_malloc(sizeof(*buf));
    buf->memsize = (uint64_t) length;

    allocate_buffer(buf, (uint64_t *)prefered_addr);

    if (buf->valid != ASE_BUFFER_VALID || buf->vbase == (uint64_t)MAP_FAILED ||
                buf->fake_paddr == 0) {
        printf("Error Allocating ASE buffer.\n");
        return FPGA_NO_MEMORY;
    }
    else {
        *buf_addr = (void*)buf->vbase;
        return FPGA_OK;
    }
}

fpga_result vai_afu_free_region(struct vai_afu_conn *conn, void *buf_addr)
{
    UNUSED_PARAM(conn);

    if (!deallocate_buffer_by_vbase((uint64_t)buf_addr)) {
        printf("deallocate error\n");
        return FPGA_EXCEPTION;
    }

    return FPGA_OK;
}

fpga_result vai_afu_mmio_read(struct vai_afu_conn *conn, uint64_t offset,
            uint64_t *value)
{
    UNUSED_PARAM(conn);

    if (mmio_afu_vbase == NULL) {
        return FPGA_INVALID_PARAM;
    }
    else {
        if (offset % sizeof(uint64_t) != 0) {
            FPGA_MSG("Misaligned MMIO access");
            return FPGA_INVALID_PARAM;
        }
        else {
            if (offset > MMIO_AFU_OFFSET) {
                FPGA_MSG("Offset out of bounds");
                return FPGA_INVALID_PARAM;
            }
            mmio_read64(offset, value);
            return FPGA_OK;
        }
    }
}

fpga_result vai_afu_mmio_write(struct vai_afu_conn *conn, uint64_t offset,
            uint64_t value)
{
    UNUSED_PARAM(conn);

    if (mmio_afu_vbase == NULL) {
        return FPGA_INVALID_PARAM;
    }
    else {
        if (offset % sizeof(uint64_t) != 0) {
            FPGA_MSG("Misaligned MMIO access");
            return FPGA_INVALID_PARAM;
        }
        else {
            if (offset > MMIO_AFU_OFFSET) {
                FPGA_MSG("Offset out of bounds");
                return FPGA_INVALID_PARAM;
            }
            mmio_write64(offset, value);
            return FPGA_OK;
        }
    }
}

fpga_result vai_afu_reset(struct vai_afu_conn *conn)
{
    UNUSED_PARAM(conn);
    send_swreset();
    return FPGA_OK;
}
