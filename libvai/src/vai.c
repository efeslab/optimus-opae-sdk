#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <stdlib.h>

#include "vai_types.h"
#include "malloc_internal.h"
#include "vai_internal.h"

#define MMIO_SPACE_LENGTH 0x1000
#define VAI_FILE_PATH "/dev/vai"
struct vai_afu_conn *vai_afu_connect()
{
    struct vai_afu_conn *conn = NULL;
    afu_id_t afu_id;
    int ret;

    conn = malloc(sizeof(*conn));
    if (!conn)
        goto err_out;

    conn->type = VAI_AFU_HW;
    conn->hw.fd = open(VAI_FILE_PATH, O_RDWR);
    if (conn->hw.fd < 0)
        goto err_free_conn;

    ret = ioctl(conn->hw.fd, VAI_GET_AFU_ID, &afu_id);
    if (ret) {
        printf("vai: ioctl returns %d\n", ret);
        goto err_close_fd;
    }

    conn->afu_id = afu_id;
    conn->hw.desc = NULL;
	conn->hw.mp = create_mspace(0, 1, conn);
    conn->hw.bar = (volatile uint64_t *)mmap(NULL, MMIO_SPACE_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, conn->hw.fd, 0);
    if (conn->hw.bar == MAP_FAILED) {
        perror("vai: map mmio bar failed");
        goto err_close_fd;
    }

    return conn;

err_close_fd:
    close(conn->hw.fd);
err_free_conn:
    free(conn);
err_out:
    return NULL;
}

fpga_result vai_afu_disconnect(struct vai_afu_conn *conn)
{
    if (!conn)
        return FPGA_OK;

    if (conn->hw.bar)
        if (munmap((void*)conn->hw.bar, MMIO_SPACE_LENGTH) != 0)
            perror("vai: unmap mmio bar failed");

    if (conn->hw.fd >= 0)
        close(conn->hw.fd);

    if (conn->hw.desc)
        free(conn->hw.desc);

    return FPGA_OK;
}

fpga_result vai_afu_map_region(struct vai_afu_conn *conn,
            uint64_t start_addr, uint64_t length)
{
    struct vai_map_info info;
    int ret;

    if (!VAI_IS_PAGE_ALIGNED(start_addr) || !(VAI_IS_PAGE_ALIGNED(length)))
        return FPGA_INVALID_PARAM;

    info.user_addr = start_addr;
    info.length = length;

    ret = ioctl(conn->hw.fd, VAI_DMA_MAP_REGION, &info);
    if (ret) {
        printf("vai: ioctl returns %d\n", ret);
        goto err_out;
    }

    return FPGA_OK;

err_out:
    return FPGA_EXCEPTION;
}

fpga_result vai_afu_unmap_region(struct vai_afu_conn *conn,
            uint64_t start_addr, uint64_t length)
{
    struct vai_map_info info;
    int ret;

    if (!VAI_IS_PAGE_ALIGNED(start_addr) || !(VAI_IS_PAGE_ALIGNED(length)))
        return FPGA_INVALID_PARAM;
 
    info.user_addr = start_addr;
    info.length = length;

    ret = ioctl(conn->hw.fd, VAI_DMA_UNMAP_REGION, &info);
    if (ret) {
        printf("vai: ioctl returns %d\n", ret);
        goto err_out;
    }

    return FPGA_OK;

err_out:
    return FPGA_EXCEPTION;
}

void vai_afu_set_mem_base(struct vai_afu_conn *conn, uint64_t mem_base) {
	ioctl(conn->hw.fd, VAI_SET_MEM_BASE, mem_base);
	// ioctl for set mem_base always return 0, because vai_b1w64_mmio return nothing
}

volatile void *vai_afu_malloc(struct vai_afu_conn *conn, uint64_t size) {
	if (!conn)
		return NULL;
	else
		return (volatile void *)mspace_malloc(conn->hw.mp, size);
}

fpga_result vai_afu_free(struct vai_afu_conn *conn, volatile void *p) {
	if (conn)
		mspace_free(conn->hw.mp, (void *)p);
    return FPGA_OK;
}

fpga_result vai_afu_mmio_read(struct vai_afu_conn *conn, uint64_t offset, uint64_t *value) {
    if (conn == NULL || conn->hw.bar == NULL || offset > MMIO_SPACE_LENGTH)
        return FPGA_INVALID_PARAM;
    *value = conn->hw.bar[offset/8];
    return FPGA_OK;
}

fpga_result vai_afu_mmio_write(struct vai_afu_conn *conn, uint64_t offset, uint64_t value) {
    if (conn == NULL || conn->hw.bar == NULL || offset > MMIO_SPACE_LENGTH)
        return FPGA_INVALID_PARAM;
    conn->hw.bar[offset/8] = value;
    return FPGA_OK;
}

fpga_result vai_afu_reset(struct vai_afu_conn *conn) {
    int ret;

    ret = ioctl(conn->hw.fd, VAI_SET_RESET);
    if (ret)
        return FPGA_EXCEPTION;

    return FPGA_OK;
}
