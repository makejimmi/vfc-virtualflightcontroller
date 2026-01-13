#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

#include "oledc.h"
#include "tc275_spi_wrap.h"
#include "tc275_gtm_wrap.h"
#include "tc275_uart_app.h"
#include "aurix_pin_mappings.h"

extern IfxCpu_syncEvent cpuSyncEvent;

float roll, pitch, yaw;
float thrust, thrust_setpoint, thrust_control_out;
float pwm_M0, pwm_M1, pwm_M2, pwm_M3;
uint8_t report_buf[36];

#if !defined(IFX_CFG_RETURN_FROM_MAIN)
void core2_main (void)
#else
int core2_main (void)
#endif
{
    roll = pitch = yaw = 0.0f;
    thrust = thrust_setpoint = thrust_control_out = 0.0f;
    pwm_M0 = pwm_M1 = pwm_M2 = pwm_M3 = 0.0f;

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
    qspi1_m_init_pins();
    tc275_spi_init();
    oledc_init(&oledc);

    oledc_image(&oledc, oledc_background, 0, 0);

    // gtm for screen refresh
    tc275_gtm_init();

    #if !defined(IFX_CFG_RETURN_FROM_MAIN)
    while (1)
    {
        if (report_gtm_flag) // report to WebSerializer - 100ms period
        {
            memset(report_buf, 0, sizeof(report_buf));
            sprintf(report_buf, "ROLL=%1.3f PITCH=%1.3f YAW=%1.3f", roll, pitch, yaw);
            uart_sendMessage(&report_buf[0], strlen(report_buf));
        }

        if (control_gtm_flag) // control thrust for pwm - 100ms period
        {
            // Calculate needed thrust to level the 'copter
            thrust_control_out = control(thrust_setpoint - thrust);

            // Calculate the four duty cycles from angles
            pwm_M0 = thrust_control_out - roll + pitch + yaw;
            pwm_M1 = thrust_control_out + roll - pitch + yaw;
            pwm_M2 = thrust_control_out + roll + pitch - yaw;
            pwm_M3 = thrust_control_out - roll - pitch - yaw;

            control_gtm_flag = FALSE;
        }
    }
#else
    return 0;
#endif
}
