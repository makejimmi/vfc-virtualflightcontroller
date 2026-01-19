#ifndef PERIPHERALWRAPPER_IMU_I2C_APP_H_
#define PERIPHERALWRAPPER_IMU_I2C_APP_H_

/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/

#include "Ifx_Types.h"
#include "IfxI2c_I2c.h"
#include "IfxPort.h"
#include "IfxStm.h"
#include "Bsp.h"


/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/

// I2C configuration
#define IMU_I2C_BAUDRATE                    400000U

#define IMU_I2C_SCL_PIN                     IfxI2c0_SCL_P02_5_INOUT
#define IMU_I2C_SDA_PIN                     IfxI2c0_SDA_P02_4_INOUT

#define IMU_I2C_ADDR_7BIT                   0x68U      //  AD0 = 0, AD1 = 0x69 if adresspin is connected VCC

// IMU register map (ICM-42688-P, User Bank 0)
#define IMU_REG_TEMP_DATA1                  0x1DU

// Burst read
#define IMU_BURST_LENGTH_BYTES              14U


// Power management (ICM-42688-P, Bank 0) - Important to start the measurements
#define IMU_REG_PWR_MGMT0                 (0x4EU)

// PWR_MGMT0 modes in  low noise configuration
#define IMU_PWR_MGMT0_ACCEL_MODE_LN       (0x03U)  // bits [1:0] = 0b11
#define IMU_PWR_MGMT0_GYRO_MODE_LN        (0x0CU)  // bits [3:2] = 0b11 -> (0x03 << 2)

// Combined enable value */
#define IMU_PWR_MGMT0_ENABLE_LN           (IMU_PWR_MGMT0_ACCEL_MODE_LN | IMU_PWR_MGMT0_GYRO_MODE_LN)

// Startup delay after enabling sensors
#define IMU_STARTUP_DELAY_MS              (10U)

// config register for acc & gyro measurement
#define IMU_REG_GYRO_CONFIG0              (0x4FU)
#define IMU_REG_ACCEL_CONFIG0             (0x50U)

// ODR - IMU internal measurement speed => 1kHz
#define IMU_ODR_1KHZ_CODE                 (0x06U)

// FS - IMU internal setting for scale and precisison => 2000dps & 16g
#define IMU_GYRO_FS_2000DPS_CODE          (0x00U)
#define IMU_ACCEL_FS_16G_CODE             (0x00U)

// offset in IMU Config0 Byte
#define IMU_CONFIG0_FS_SHIFT              (5U)

// setting values into the register
#define IMU_GYRO_CONFIG0_VALUE            ((IMU_GYRO_FS_2000DPS_CODE << IMU_CONFIG0_FS_SHIFT) | (IMU_ODR_1KHZ_CODE))
#define IMU_ACCEL_CONFIG0_VALUE           ((IMU_ACCEL_FS_16G_CODE    << IMU_CONFIG0_FS_SHIFT) | (IMU_ODR_1KHZ_CODE))

// physical constants for calculation acc & G-force
#define IMU_GYRO_LSB_PER_DPS              (16.4f)
#define IMU_ACCEL_LSB_PER_G               (2048.0f)
#define IMU_G_TO_MS2                      (9.80665f)
#define IMU_DEG_TO_RAD                    (0.017453292519943295f)

// constants for temperature calculation
#define IMU_TEMP_SENS_LSB_PER_C           (132.48f)
#define IMU_TEMP_OFFSET_C                 (25.0f)



/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*-------------------------------------------------Data Structures---------------------------------------------------*/
/*********************************************************************************************************************/

typedef struct
{
    sint16 temp;
    sint16 ax;
    sint16 ay;
    sint16 az;
    sint16 gx;
    sint16 gy;
    sint16 gz;
} ImuDataRaw_t;

/*********************************************************************************************************************/
/*--------------------------------------------Private Variables/Constants--------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/

void    IMU_I2C_Init(void);
boolean IMU_I2C_ReadAll(ImuDataRaw_t *data);

static inline float IMU_Accel_g(sint16 raw)
{
    return ((float)raw / IMU_ACCEL_LSB_PER_G);
}

static inline float IMU_Accel_ms2(sint16 raw)
{
    return (IMU_Accel_g(raw) * IMU_G_TO_MS2);
}

static inline float IMU_Gyro_dps(sint16 raw)
{
    return ((float)raw / IMU_GYRO_LSB_PER_DPS);
}

static inline float IMU_Gyro_rad_s(sint16 raw)
{
    return (IMU_Gyro_dps(raw) * IMU_DEG_TO_RAD);
}

static inline float IMU_Temp_C(sint16 raw)
{
    return (((float)raw / IMU_TEMP_SENS_LSB_PER_C) + IMU_TEMP_OFFSET_C);
}


#endif /* PERIPHERALWRAPPER_IMU_I2C_APP_H_ */
