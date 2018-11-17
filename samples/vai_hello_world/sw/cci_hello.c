#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <uuid/uuid.h>

#include <vai/vai.h>

int main(int argc, char *argv[])
{
    struct vai_afu_conn *conn = vai_afu_connect();
    void *buf_addr;
    volatile char *buf;

    buf_addr = vai_afu_malloc(conn, 4096);

	printf("buf_addr %p\n", buf_addr);

    buf = (char*)buf_addr;
    buf[0] = 0;

    vai_afu_mmio_write(conn, 0, (uint64_t)buf/CL(1));

    while (0 == buf[0]) {

    }

    printf("%s\n", buf);

	vai_afu_free(conn, buf);

    vai_afu_disconnect(conn);

    return 0;
}
