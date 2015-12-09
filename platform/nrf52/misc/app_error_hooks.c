#include "app_error.h"
#include "io.h"

void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name){
    LOGE("NRF error %u, line %u, %s\r\n", error_code, line_num, p_file_name);
}
