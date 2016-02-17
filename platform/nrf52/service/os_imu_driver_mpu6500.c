#include "os_imu_driver.h"
#include "crypto.h"
#include "nrf_drv_spi.h"
#include "cmsis_os.h"
#include "io.h"
#include "util.h"
#include "mpu_6500_registers.h"

#define MPU_READ_ADDR(d) (d | 0x80)
#define MPU_WRITE_ADDR(d) (d & 0x7F)

static nrf_drv_spi_t _spi_context;
static const nrf_drv_spi_t _spi_master = NRF_DRV_SPI_INSTANCE(0);
static os_imu_config_t _config;

static osStatus _spi_write_byte(uint8_t address, uint8_t value){
    uint8_t spi_buf[2] = {0};//maybe it's not needed?
    spi_buf[0] = MPU_WRITE_ADDR(address);
    spi_buf[1] = value;
    uint32_t ret;
    ret =  nrf_drv_spi_transfer(&_spi_master, spi_buf, sizeof(spi_buf), NULL, 0);
    if( ret == NRF_SUCCESS ){
        return osOK;
    }else{
        LOGE("SPI Write Error %u\r\n", ret);
    }
    return osErrorResource;
}
static osStatus _spi_read_byte(uint8_t address, uint8_t * out){
    uint8_t spi_buf[2] = {MPU_READ_ADDR(address), 0};
    uint32_t ret;
    ret =  nrf_drv_spi_transfer(&_spi_master, spi_buf, sizeof(spi_buf), spi_buf, sizeof(spi_buf));
    if( ret == NRF_SUCCESS ){
        *out = spi_buf[1];
        return osOK;
    }else{
        LOGE("SPI Read Error %u\r\n", ret);
    }
    return osErrorResource;
}
static void _imu_reset_signal(void){
    ASSERT_OK(_spi_write_byte(
                MPU_REG_USER_CTL,
                (USR_CTL_I2C_DIS | USR_CTL_FIFO_RST | USR_CTL_SIG_RST)
                ));
    ASSERT_OK(_spi_write_byte(
                MPU_REG_SIG_RST,
                (0xFF)
                ));
    ASSERT_OK(_spi_write_byte(
                PWR_MGMT_1_RESET,
                ( PWR_MGMT_1_RESET )
                ));
}
static _imu_config_normal_mode(const os_imu_config_t * config){

}
osStatus os_imu_driver_init(const os_imu_config_t * config){
    //configure driver
    nrf_drv_spi_config_t spi_config =(nrf_drv_spi_config_t){                                                            \
        .sck_pin      = SPI0_CONFIG_SCK_PIN,
        .mosi_pin     = SPI0_CONFIG_MOSI_PIN,
        .miso_pin     = SPI0_CONFIG_MISO_PIN,
        .ss_pin       = SPI0_CONFIG_CS_PIN,
        .irq_priority = SPI0_CONFIG_IRQ_PRIORITY,
        .orc          = 0xFF,
        .frequency    = NRF_DRV_SPI_FREQ_1M,
        .mode         = NRF_DRV_SPI_MODE_0,
        .bit_order    = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST,
    };
    if(NRF_SUCCESS != nrf_drv_spi_init(&_spi_master,&spi_config, NULL)){
        return osErrorResource;
    }
    //copy user supplied config parameter
    _config = *config;

    return os_imu_driver_reset();
}

osStatus os_imu_driver_reset(void){
    //turn off IMU, if any
    //wait 100 ms
    osDelay(100);
    //read ID
    {
        uint8_t buf = 0;
        ASSERT_OK(_spi_read_byte(MPU_REG_WHO_AM_I, &buf));
        if ( buf != CHIP_ID ){
            LOGE("Chip ID mismatch, expect %x, got %x.\r\n", CHIP_ID, buf);
            return osErrorResource;
        }
    }
    _imu_reset_signal();
    _imu_config_normal_mode(&_config);
    return osOK;
}

osStatus os_imu_driver_read(os_imu_data_t * out_data){
    uint8_t accel[6] = {0};
    ASSERT_OK(_spi_read_byte(MPU_REG_ACC_X_HI, accel+0));
    ASSERT_OK(_spi_read_byte(MPU_REG_ACC_X_LO, accel+1));
    ASSERT_OK(_spi_read_byte(MPU_REG_ACC_Y_HI, accel+2));
    ASSERT_OK(_spi_read_byte(MPU_REG_ACC_Y_LO, accel+3));
    ASSERT_OK(_spi_read_byte(MPU_REG_ACC_Z_HI, accel+4));
    ASSERT_OK(_spi_read_byte(MPU_REG_ACC_Z_LO, accel+5));
    out_data->x = (uint32_t)accel[0] << 8 + accel[1];
    out_data->y = (uint32_t)accel[2] << 8 + accel[3];
    out_data->z = (uint32_t)accel[4] << 8 + accel[5];
    /*
     *out_data->x = os_rand() % 64;
     *out_data->y = os_rand() % 64;
     *out_data->z = os_rand() % 64;
     */
    return osOK;
}
