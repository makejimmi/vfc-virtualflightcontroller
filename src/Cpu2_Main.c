#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

#include "oledc.h"
#include "tc275_spi_wrap.h"
#include "tc275_gtm_wrap.h"
#include "tc275_uart_app.h"
#include "aurix_pin_mappings.h"
#include "controller.h"
#include <string.h>
#include <stdio.h>

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
            sprintf((char *)report_buf, "ROLL=%1.3f PITCH=%1.3f YAW=%1.3f\r\n", roll, pitch, yaw);
            uart_sendMessage(&report_buf[0], strlen((char *)report_buf));
        }

        if (control_gtm_flag) // control thrust for pwm - 100ms period
        {
            // Calculate needed thrust to level the 'copter
            thrust_control_out = controller(thrust_setpoint - thrust);

            // Calculate the four duty cycles from angles
            // CAUTION: M0 is top right, M1 bottom left, M2 top left, M3 in the remaining space
            pwm_M0 = thrust_control_out - roll + pitch + yaw;
            pwm_M1 = thrust_control_out + roll - pitch + yaw;
            pwm_M2 = thrust_control_out + roll + pitch - yaw;
            pwm_M3 = thrust_control_out - roll - pitch - yaw;

            /*
            if (pwm_M0 > 0.75f) {
                M0_img = M0_75_img;
            } else if (pwm_M0 > 0.5f) {
                M0_img = M0_50_img;
            } else if (pwm_M0 > 0.25f) {
                M0_img = M0_25_img;
            } else {
                M0_img = M0_00_img;
            }
            oledc_image(&oledc, M0_img, 96-10, 10);

            if (pwm_M0 > 0.75f) {
                M1_img = M1_75_img;
            } else if (pwm_M0 > 0.5f) {
                M1_img = M1_50_img;
            } else if (pwm_M0 > 0.25f) {
                M1_img = M1_25_img;
            } else {
                M1_img = M1_00_img;
            }
            oledc_image(&oledc, M1_img, 10, 96-10);

            if (pwm_M0 > 0.75f) {
                M2_img = M2_75_img;
            } else if (pwm_M0 > 0.5f) {
                M2_img = M2_50_img;
            } else if (pwm_M0 > 0.25f) {
                M2_img = M2_25_img;
            } else {
                M2_img = M2_00_img;
            }
            oledc_image(&oledc, M2_img, 10, 10);

            if (pwm_M0 > 0.75f) {
                M3_img = M3_75_img;
            } else if (pwm_M0 > 0.5f) {
                M3_img = M3_50_img;
            } else if (pwm_M0 > 0.25f) {
                M3_img = M3_25_img;
            } else {
                M3_img = M3_00_img;
            }
            oledc_image(&oledc, M3_img, 96-10, 96-10);
*/

            control_gtm_flag = FALSE;
        }
    }
#else
    return 0;
#endif
}
