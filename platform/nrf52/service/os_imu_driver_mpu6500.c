#include "os_imu_driver.h"
#include "crypto.h"
#include "nrf_drv_spi.h"

static nrf_drv_spi_t _spi_context;
static const nrf_drv_spi_t _spi_master = NRF_DRV_SPI_INSTANCE(0);

void _spi_handler(nrf_drv_spi_event_t event){

}
osStatus os_imu_driver_init(const os_imu_config_t * config){
    //first, use sensible defaults
    nrf_drv_spi_config_t spi_config =(nrf_drv_spi_config_t)NRF_DRV_SPI_DEFAULT_CONFIG(0);
    //override necessary defaults from user config
    //None at this moment
    if(NRF_SUCCESS != nrf_drv_spi_init(&_spi_master,&spi_config, _spi_handler)){
        return osErrorResource;
    }
    return osOK;
}

osStatus os_imu_driver_reset(void){
    return osOK;
}

osStatus os_imu_driver_read(os_imu_data_t * out_data){
    out_data->x = os_rand() % 64;
    out_data->y = os_rand() % 64;
    out_data->z = os_rand() % 64;
    return osOK;
}
