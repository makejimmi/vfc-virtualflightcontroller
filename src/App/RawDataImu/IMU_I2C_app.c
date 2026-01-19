//Beschreibung ergänzen

/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/

#include "IMU_I2C_app.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/

/* Convert MSB/LSB to signed 16-bit */
#define IMU_BE16(msb, lsb)      ((sint16)((((uint16)(msb)) << 8) | ((uint16)(lsb))))

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/


/*********************************************************************************************************************/
/*--------------------------------------------Private Variables/Constants--------------------------------------------*/
/*********************************************************************************************************************/

static IfxI2c_I2c        g_i2c;
static IfxI2c_I2c_Device g_i2cDevice;

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/

void IMU_I2C_Init(void)
{
    /* ---- I2C module configuration ---- */
    IfxI2c_I2c_Config i2cConfig;
    IfxI2c_I2c_initConfig(&i2cConfig, &MODULE_I2C0);

    /* Pin configuration (wie im SHT-Beispiel) */
    static const IfxI2c_Pins i2cPins =
    {
        &IMU_I2C_SCL_PIN,
        &IMU_I2C_SDA_PIN,
        IfxPort_PadDriver_ttlSpeed1
    };

    i2cConfig.pins     = &i2cPins;
    i2cConfig.baudrate = IMU_I2C_BAUDRATE;

    IfxI2c_I2c_initModule(&g_i2c, &i2cConfig);

    /* ---- I2C device configuration ---- */
    IfxI2c_I2c_deviceConfig devConfig;
    IfxI2c_I2c_initDeviceConfig(&devConfig, &g_i2c);

    /* 7-bit address (AD0=0 -> 0x68) */
    devConfig.deviceAddress = IMU_I2C_ADDR_7BIT << 1U;

    IfxI2c_I2c_initDevice(&g_i2cDevice, &devConfig);

    /* Enable Accel + Gyro (Low-Noise mode) */
    {
        uint8 tx[2];
        tx[0] = (uint8)IMU_REG_PWR_MGMT0;
        tx[1] = (uint8)IMU_PWR_MGMT0_ENABLE_LN;

        while (IfxI2c_I2c_write(&g_i2cDevice, tx, 2U) == IfxI2c_I2c_Status_nak)
        {
            /* retry on NAK */
        }

        /* Startup Time */
        waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, IMU_STARTUP_DELAY_MS));
    }

    /* Set Gyro Full-Scale + ODR */
    {
        uint8 tx[2];
        tx[0] = (uint8)IMU_REG_GYRO_CONFIG0;
        tx[1] = (uint8)IMU_GYRO_CONFIG0_VALUE;

        while (IfxI2c_I2c_write(&g_i2cDevice, tx, 2U) == IfxI2c_I2c_Status_nak)
        {
            /* retry on NAK */
        }
    }

    /* Set Accel Full-Scale + ODR */
    {
        uint8 tx[2];
        tx[0] = (uint8)IMU_REG_ACCEL_CONFIG0;
        tx[1] = (uint8)IMU_ACCEL_CONFIG0_VALUE;

        while (IfxI2c_I2c_write(&g_i2cDevice, tx, 2U) == IfxI2c_I2c_Status_nak)
        {
            /* retry on NAK */
        }
    }
}


boolean IMU_I2C_ReadAll(ImuDataRaw_t *data)
{
    uint8 buf[IMU_BURST_LENGTH_BYTES];
    uint8 startReg = IMU_REG_TEMP_DATA1;

    if (data == NULL)
    {
        return FALSE;
    }

    /* Write start register */
    if (IfxI2c_I2c_write(&g_i2cDevice, &startReg, 1) != IfxI2c_I2c_Status_ok)
    {
        return FALSE;
    }

    /* Read burst data */
    if (IfxI2c_I2c_read(&g_i2cDevice, buf, IMU_BURST_LENGTH_BYTES) != IfxI2c_I2c_Status_ok)
    {
        return FALSE;
    }

    data->temp = IMU_BE16(buf[0],  buf[1]);

    data->ax   = IMU_BE16(buf[2],  buf[3]);
    data->ay   = IMU_BE16(buf[4],  buf[5]);
    data->az   = IMU_BE16(buf[6],  buf[7]);

    data->gx   = IMU_BE16(buf[8],  buf[9]);
    data->gy   = IMU_BE16(buf[10], buf[11]);
    data->gz   = IMU_BE16(buf[12], buf[13]);

    return TRUE;
}

