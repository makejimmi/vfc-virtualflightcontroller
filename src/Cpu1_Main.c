#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

#include "src/app/AngleCalculation/Angle_calculation.h"
#include <tc275_uart_app.h>
#include <string.h>
#include <stdio.h>

extern IfxCpu_syncEvent cpuSyncEvent;

/* --------------------(START)Remove in finished project-------------------- */
#define UART_TX_BUFFER_SIZE (200U)
/* --------------------(END) Remove in finished project-------------------- */

#define OFFSET_CALC_WAIT_TIME_MS   50
#define OFFSET_CALC_SIZE   100

#if !defined(IFX_CFG_RETURN_FROM_MAIN)
void core1_main (void)
#else
int core1_main (void)
#endif
{
    /* Create Data struct and read, using shared mem manager function with mutex */
    ImuData_t imu_data_holder;
    ImuData_t imu_offset_values;
    ImuData_t imu_data;

    OrientationData_t angles;
    angles.pitch = 0;
    angles.roll  = 0;
    angles.yaw   = 0;

    /* --------------------(START)Remove in finished project-------------------- */
//    initUART(); /* Initialize the UART module      */
//    const uint8 testMsg[] = "Core1: TestMessage\r\n";

//    uart_sendMessage((uint8 *)testMsg, (Ifx_SizeT)strlen((char *)testMsg));
//    uint8 uartBuf_ang[UART_TX_BUFFER_SIZE];
    int len = 0;
    /* --------------------(END) Remove in finished project-------------------- */


    IfxCpu_enableInterrupts();
    /*
     * !!WATCHDOG1 IS DISABLED HERE!!
     * Enable the watchdog in the demo if it is required and also service the watchdog periodically
     * */
    IfxScuWdt_disableCpuWatchdog (IfxScuWdt_getCpuWatchdogPassword ());

    /* Cpu sync event wait*/
    IfxCpu_emitEvent(&cpuSyncEvent);
    IfxCpu_waitEvent(&cpuSyncEvent, 1);


    /* ------GET anglespeed offset values-------- */
    imu_offset_values.accel_x = 0;
    imu_offset_values.accel_y = 0;
    imu_offset_values.accel_z = 0;
    imu_offset_values.gyro_x = 0;
    imu_offset_values.gyro_y = 0;
    imu_offset_values.gyro_z = 0;

    for(int idx = 0; idx < OFFSET_CALC_SIZE; idx++){
        imu_data_holder = SharedMem_ReadImuData_C0_C1();
        imu_offset_values.accel_x = imu_offset_values.accel_x + imu_data_holder.accel_x;
        imu_offset_values.accel_y = imu_offset_values.accel_y + imu_data_holder.accel_y;
        imu_offset_values.accel_z = imu_offset_values.accel_z + imu_data_holder.accel_z;
        imu_offset_values.gyro_x  = imu_offset_values.gyro_x  + imu_data_holder.gyro_x;
        imu_offset_values.gyro_y  = imu_offset_values.gyro_y  + imu_data_holder.gyro_y;
        imu_offset_values.gyro_z  = imu_offset_values.gyro_z  + imu_data_holder.gyro_z;
        waitTime(IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, OFFSET_CALC_WAIT_TIME_MS));
    }

    imu_offset_values.accel_x = imu_offset_values.accel_x / OFFSET_CALC_SIZE;
    imu_offset_values.accel_y = imu_offset_values.accel_y / OFFSET_CALC_SIZE;
    imu_offset_values.accel_z = imu_offset_values.accel_z / OFFSET_CALC_SIZE;
    imu_offset_values.gyro_x  = imu_offset_values.gyro_x  / OFFSET_CALC_SIZE;
    imu_offset_values.gyro_y  = imu_offset_values.gyro_y  / OFFSET_CALC_SIZE;
    imu_offset_values.gyro_z  = imu_offset_values.gyro_z  / OFFSET_CALC_SIZE;
    /* ----offset calc done----*/


#if !defined(IFX_CFG_RETURN_FROM_MAIN)
    while (1)
    {
        /* Get new data and subtract offset*/
        imu_data = SharedMem_ReadImuData_C0_C1();

        imu_data.accel_x -= imu_offset_values.accel_x;
        imu_data.accel_y -= imu_offset_values.accel_y;
        //imu_data.accel_z -= imu_offset_values.accel_z;
        imu_data.gyro_x  -= imu_offset_values.gyro_x;
        imu_data.gyro_y  -= imu_offset_values.gyro_y;
        imu_data.gyro_z  -= imu_offset_values.gyro_z;

        /* roll = x, pitch = y, yaw = z */
        /* complemtary filter only gives roll and pitch */
        angles = angle_calc(imu_data);

        /* write angles to CPU2 */
        SharedMem_WriteOrientation_C1_C2(angles);

        /* --------------------(START)Remove in finished project-------------------- */
        /*
        len = snprintf((char *)uartBuf_ang, UART_TX_BUFFER_SIZE,
                                       "CPU1 Angles: Roll: %.3f  Pitch %.3f  Yaw %.3f\r\n",
                                       angles.roll, angles.pitch, angles.yaw);
*/
    //    uart_sendMessage((uint8 *)uartBuf_ang, (Ifx_SizeT)len);
        /* --------------------(END) Remove in finished project-------------------- */
        angle_calc_wait_sample_time();
    }
#else
return 0;
#endif
}
