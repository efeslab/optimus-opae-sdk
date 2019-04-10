#include <vai/wrapper.h>
#include <vai/fpga.h>
#include <stdio.h>

#define TOKEN_MAGIC ((void*)(0x2333666))
fpga_result fpgaEnumerate(const fpga_properties *filters,
			  uint32_t num_filters, fpga_token *tokens,
			  uint32_t max_tokens, uint32_t *num_matches)
{
    UNUSED_PARAM(filters);
    UNUSED_PARAM(num_filters);
    UNUSED_PARAM(tokens);

    if (max_tokens == 0)
        return FPGA_EXCEPTION;

    *tokens = TOKEN_MAGIC;
    *num_matches = 1;
    return FPGA_OK;
}

fpga_result fpgaDestroyToken(fpga_token *token)
{
    *token = 0;
    return FPGA_OK;
}

fpga_result fpgaOpen(fpga_token token, fpga_handle *handle, int flags)
{
    struct vai_afu_conn *conn;

    UNUSED_PARAM(flags);

    if (token != TOKEN_MAGIC)
        return FPGA_EXCEPTION;

    conn = vai_afu_connect();
    if (conn != NULL) {
        *handle = conn;
        return FPGA_OK;
    }

    return FPGA_EXCEPTION;
}

fpga_result fpgaClose(fpga_handle handle)
{
    struct vai_afu_conn *conn = handle;
    return vai_afu_disconnect(conn);
}

fpga_result fpgaReset(fpga_handle handle)
{
    struct vai_afu_conn *conn = handle;
    return vai_afu_reset(conn);
}

fpga_result fpgaPrepareBuffer(fpga_handle handle,
			      uint64_t len,
			      void **buf_addr, uint64_t *wsid, int flags)
{
    struct vai_afu_conn *conn = handle;
    volatile void *buf;

    UNUSED_PARAM(flags);

    buf = vai_afu_malloc(conn, len);
    *wsid = (uint64_t)buf;
    *buf_addr = (void*)buf;

    if (buf)
        return FPGA_OK;
    else
        return FPGA_EXCEPTION;
}

fpga_result fpgaReleaseBuffer(fpga_handle handle, uint64_t wsid)
{
    struct vai_afu_conn *conn = handle;
    void *buf = (void*)wsid;

    return vai_afu_free(conn, buf);
}

fpga_result fpgaGetIOAddress(fpga_handle handle, uint64_t wsid,
			     uint64_t *ioaddr)
{
    UNUSED_PARAM(handle);

    *ioaddr = wsid;
    return FPGA_OK;
}


fpga_result fpgaWriteMMIO64(fpga_handle handle,
            uint32_t mmio_num, uint64_t offset,
            uint64_t value)
{
    struct vai_afu_conn *conn = handle;

    UNUSED_PARAM(mmio_num);

    return vai_afu_mmio_write(conn, offset, value);
}

fpga_result fpgaReadMMIO64(fpga_handle handle,
            uint32_t mmio_num, uint64_t offset,
            uint64_t *value)
{
    struct vai_afu_conn *conn = handle;

    UNUSED_PARAM(mmio_num);

    return vai_afu_mmio_read(conn, offset, value);
}
