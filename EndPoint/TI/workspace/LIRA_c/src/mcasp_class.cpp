#include "mcasp_class.hpp"

mcasp_ts::mcasp_ts(uint8_t pru_num, uint8_t mcasp_num, uint8_t ts_idx)
{
    this->file_name << "/dev/" << "mcasp" << std::to_string(mcasp_num) << "_pru" << std::to_string(pru_num) << "_ts" << std::to_string(ts_idx);
    PRINTF(MAG, "Opening file : %s.\n", file_name.str().c_str());
    this->fd = open(file_name.str().c_str(), O_RDWR | S_IWUSR | S_IRUSR );
    if (fd < 0) { FPRINTF_EXT("Can't open file '%s'.\r\n", file_name.str().c_str()); goto failure; }
    this->send_buf              = (int32_t *)malloc(SEND_BUF_SIZE_BYTES);
    this->send_buf_byte_size    = SEND_BUF_SIZE_BYTES;
    this->recv_buf              = (int32_t *)malloc(RECV_BUF_SIZE_BYTES);
    this->recv_buf_byte_size    = RECV_BUF_SIZE_BYTES;
    return;

failure:
    delete this;
    return;
}

mcasp_ts::~mcasp_ts(void)
{
    if (this->fd >= 0)
        close(this->fd);
    if(this->send_buf) { free(this->send_buf); this->send_buf = NULL; }
    if(this->recv_buf) { free(this->recv_buf); this->recv_buf = NULL; }
}

void mcasp_ts::write_sin(void)
{
    int32_t ret_val;

    if (this->fd < 0) goto failure;
    for ( int i = 0; i < int(SEND_BUF_SIZE_WORDS); i++ )
    {
        this->send_buf[i] = (int32_t)(20000.0 * (double) sin ( (i / SIN_PERIOD) * 2 * M_PI ) );
    }
    ret_val = write(
                    this->fd,
                    this->send_buf,
                    SEND_BUF_SIZE_BYTES
                    );
    if (ret_val < 0) FPRINTF;
    return;

failure:
    PRINT_ERR("File not opened : fd=%i.\r\n", this->fd);
    return;
}

void mcasp_ts::read_mcasp   (int32_t * recv_buf, uint32_t recv_buf_bytes_size)
{
    int32_t ret_val;
    if (this->fd < 0) goto failure;
    ret_val = read(
                    this->fd,
                    recv_buf,
                    recv_buf_bytes_size
                    );
    if (ret_val < 0) FPRINTF;
    return;

failure:
    PRINT_ERR("File not opened : fd=%i.\r\n", this->fd);
    return;
}

void mcasp_ts::write_mcasp  (int32_t * send_buf, uint32_t send_buf_bytes_size)
{
    int32_t ret_val;
    if (this->fd < 0) goto failure;
    ret_val = write(
                    this->fd,
                    send_buf,
                    send_buf_bytes_size
                    );
    if (ret_val < 0) FPRINTF;
    return;

failure:
    PRINT_ERR("File not opened : fd=%i.\r\n", this->fd);
    return;
}
