#include <ase_common.h>
/*
 * dlmalloc
 */
#include <vai/fpga.h>
#include "malloc_internal.h"
#include "vai_internal.h"

volatile void *vai_afu_malloc(struct vai_afu_conn *conn, size_t size) {
	if (conn == global_conn) {
		return (volatile void*)dlmalloc(size);
	}
	else {
		ASE_ERR("Global conn != required conn");
		return (volatile void*)NULL;
	}
}
fpga_result vai_afu_free(struct vai_afu_conn *conn, volatile void *p) {
	if (conn == global_conn) {
		dlfree((void *)p);
	}
	else {
		ASE_ERR("Global conn != required conn");
	}
    return FPGA_OK;
}
