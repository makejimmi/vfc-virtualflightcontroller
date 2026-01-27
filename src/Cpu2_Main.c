#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

#include "oledc.h"
#include "tc275_spi_wrap.h"
#include "tc275_gtm_wrap.h"
#include "tc275_uart_app.h"
#include "SharedMem/InterCoreData.h"
#include "aurix_pin_mappings.h"
#include "controller.h"
#include <string.h>
#include <stdio.h>

extern IfxCpu_syncEvent cpuSyncEvent;


#if !defined(IFX_CFG_RETURN_FROM_MAIN)
void core2_main (void)
#else
int core2_main (void)
#endif
{
    OrientationData_t orientation;

    float thrust, thrust_setpoint, thrust_torque;
    float pwm_M0, pwm_M1, pwm_M2, pwm_M3;
    uint8_t report_buf[36];
    float roll_torque, pitch_torque, yaw_torque;
    thrust = thrust_setpoint = thrust_torque = 0.0f;
    pwm_M0 = 50.0f;
    pwm_M1 = pwm_M2 = pwm_M3 = 0.0f;

    IfxCpu_enableInterrupts();
    /*
     * !!WATCHDOG2 IS DISABLED HERE!!
     * Enable the watchdog in the demo if it is required and also service the watchdog periodically
     * */
    IfxScuWdt_disableCpuWatchdog (IfxScuWdt_getCpuWatchdogPassword ());

    /* Cpu sync event wait*/
    IfxCpu_emitEvent(&cpuSyncEvent);
    IfxCpu_waitEvent(&cpuSyncEvent, 1);

    // enable SPI peripheral for OledC-Click
    gpio_init_pins();
    initUART();
    qspi1_m_init_pins();
    tc275_spi_init();
    oledc_init(&oledc);

    oledc_background_image(&oledc, oledc_background);

    // gtm for screen refresh
    tc275_gtm_init();

    #if !defined(IFX_CFG_RETURN_FROM_MAIN)
    while (1)
    {
        if (report_gtm_flag) // report to WebSerializer - 100ms period
        {
            orientation = SharedMem_ReadOrientation_C1_C2();
            orientation.roll = -orientation.roll;
            memset(report_buf, 0, sizeof(report_buf));
            sprintf((char *)report_buf, "ROLL=%1.3f PITCH=%1.3f YAW=%1.3f\r\n", orientation.roll, orientation.pitch, orientation.yaw);
            uart_sendMessage(&report_buf[0], strlen((char *)report_buf));
        }

        if (control_gtm_flag) // control thrust for pwm - 100ms period
        {
            // Calculate needed thrust to level the 'copter
            thrust_torque = controller(thrust_setpoint - thrust);
            roll_torque = controller(0.0f - orientation.roll);
            pitch_torque = controller(0.0f - orientation.pitch);
            yaw_torque = controller(0.0f - orientation.yaw);

            // Calculate the four duty cycles from angles
            // CAUTION: M0 is top right, M1 bottom left, M2 top left, M3 in the remaining space
            pwm_M0 = thrust_torque - roll_torque + pitch_torque + yaw_torque;
            pwm_M1 = thrust_torque + roll_torque - pitch_torque + yaw_torque;
            pwm_M2 = thrust_torque + roll_torque + pitch_torque - yaw_torque;
            pwm_M3 = thrust_torque - roll_torque - pitch_torque - yaw_torque;

            oledc_update_pwm_graphics(&oledc, &pwm_M0, &pwm_M1, &pwm_M2, &pwm_M3);

            control_gtm_flag = FALSE;
        }
    }
#else
    return 0;
#endif
}
