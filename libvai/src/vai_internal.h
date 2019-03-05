#ifndef _VAI_INTERNAL_H_
#define _VAI_INTERNAL_H_

#include <vai/fpga.h>

void vai_afu_set_mem_base(struct vai_afu_conn *conn, uint64_t mem_base);
fpga_result vai_afu_map_region(struct vai_afu_conn *conn,
        uint64_t start_addr, uint64_t length);

#endif
