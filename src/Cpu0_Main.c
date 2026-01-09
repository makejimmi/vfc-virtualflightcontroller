#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

#include <string.h>
#include <stdio.h>
#include <tc275_uart_app.h>

#include "src/App/SharedMem/InterCoreData.h"

IfxCpu_syncEvent cpuSyncEvent = 0;

#if !defined(IFX_CFG_RETURN_FROM_MAIN)
void core0_main(void)
#else
int core0_main(void)
#endif
{
    uint8 message[] = "a_x: x.xxx a_y: x.xxx a_z: x.xxx g_x: x.xxx g_y: x.xxx g_z: x.xxx\n";
    Ifx_SizeT msg_size = 0;

    ImuData_t example_imu_data = {
        .accel_x = 0.1f,
        .accel_y = 0.2f,
        .accel_z = 0.3f,
        .gyro_x  = 1.1f,
        .gyro_y  = 1.2f,
        .gyro_z  = 1.3f
    };

    SharedMem_WriteImuData_C0_C1(example_imu_data);

    IfxCpu_enableInterrupts();
    /*
     * !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdog in the demo if it is required and also service the watchdog periodically
     * */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    /* Cpu sync event wait*/
    IfxCpu_emitEvent(&cpuSyncEvent);
    IfxCpu_waitEvent(&cpuSyncEvent, 1);

    initUART();                          /* Initialize the UART module      */
    msg_size = strlen((char *)&message); /* determine string length      */

#if !defined(IFX_CFG_RETURN_FROM_MAIN)
    while (1)
    {
        // EXAMPLE USAGE OF SHARED MEMORY FUNCTIONS ----------------------------------------------
        ImuData_t g_example = SharedMem_ReadImuData_C0_C1(); // read data from shared memory (struct copy)
        sprintf((char *)&message[0],
                "a_x: %.3f a_y: %.3f a_z: %.3f g_x: %.3f g_y: %.3f g_z: %.3f\n",
                g_example.accel_x,
                g_example.accel_y,
                g_example.accel_z,
                g_example.gyro_x,
                g_example.gyro_y,
                g_example.gyro_z);
        msg_size = strlen((char *)&message); /* determine string length      */

        uart_sendMessage(&message[0], msg_size); /* send entire message */

        g_example.accel_x += 0.1f;
        if (g_example.accel_x > 10.0f) {
            g_example.accel_x = 0.0f;
        }
        SharedMem_WriteImuData_C0_C1(g_example); // write updated data back to shared memory
    }
#else
    return 0;
#endif
}
