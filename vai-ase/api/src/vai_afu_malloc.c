#include <ase_common.h>
/*
 * dlmalloc
 */
#include <vai/malloc.h>
#include <vai/vai.h>
void *vai_afu_malloc(struct vai_afu_conn *conn, size_t size) {
	if (conn == global_conn) {
		return dlmalloc(size);
	}
	else {
		ASE_ERR("Global conn != required conn");
		return NULL;
	}
}
void vai_afu_free(struct vai_afu_conn *conn, void *p) {
	if (conn == global_conn) {
		dlfree(p);
	}
	else {
		ASE_ERR("Global conn != required conn");
	}
}
