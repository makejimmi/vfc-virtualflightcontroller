#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

#include "oledc.h"
#include "oledc_M01_img.h"
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
            //pwm_M0 = thrust_control_out - roll + pitch + yaw;
            //pwm_M1 = thrust_control_out + roll - pitch + yaw;
            //pwm_M2 = thrust_control_out + roll + pitch - yaw;
            //pwm_M3 = thrust_control_out - roll - pitch - yaw;

            pwm_M0 += 4.0f;
            pwm_M1 -= 4.0f;
            pwm_M2 += 4.0f;
            pwm_M3 -= 4.0f;

            if (pwm_M0 > 100.0f)
                pwm_M0 = 0.0f;
            if (pwm_M2 > 100.0f)
                pwm_M2 = 0.0f;
            if (pwm_M1 < 0.0f)
                pwm_M1 = 100.0f;
            if (pwm_M3 < 0.0f)
                pwm_M3 = 100.0f;


            if (pwm_M0 > 90.0f) {
                oledc_line(&oledc, 24, 17, 24, 18, 0xFFFF);
                oledc_line(&oledc, 23, 16, 23, 19, 0xFFFF);
            } else {
                oledc_line(&oledc, 24, 17, 24, 18, 0x0000);
                oledc_line(&oledc, 23, 16, 23, 19, 0x0000);
            }
            if (pwm_M0 > 80.0f) {
                oledc_line(&oledc, 22, 15, 22, 20, 0xFFFF);
                oledc_line(&oledc, 21, 14, 21, 21, 0xFFFF);
            } else {
                oledc_line(&oledc, 22, 15, 22, 20, 0x0000);
                oledc_line(&oledc, 21, 14, 21, 21, 0x0000);
            }
            if (pwm_M0 > 70.0f) {
                oledc_line(&oledc, 20, 13, 20, 22, 0xFFFF);
                oledc_line(&oledc, 19, 12, 19, 23, 0xFFFF);
            } else {
                oledc_line(&oledc, 20, 13, 20, 22, 0x0000);
                oledc_line(&oledc, 19, 12, 19, 23, 0x0000);
            }
            if (pwm_M0 > 60.0f) {
                oledc_line(&oledc, 18, 11, 18, 24, 0xFFFF);
                oledc_line(&oledc, 17, 19, 17, 16, 0xFFFF);
            } else {
                oledc_line(&oledc, 18, 11, 18, 24, 0x0000);
                oledc_line(&oledc, 17, 19, 17, 16, 0x0000);
            }
            if (pwm_M0 > 50.0f) {
                oledc_line(&oledc, 16, 19, 16, 16, 0xFFFF);
                oledc_line(&oledc, 15, 19, 15, 16, 0xFFFF);
            } else {
                oledc_line(&oledc, 16, 19, 16, 16, 0x0000);
                oledc_line(&oledc, 15, 19, 15, 16, 0x0000);
            }
            if (pwm_M0 > 40.0f) {
                oledc_line(&oledc, 14, 19, 14, 16, 0xFFFF);
                oledc_line(&oledc, 13, 19, 13, 16, 0xFFFF);
            } else {
                oledc_line(&oledc, 14, 19, 14, 16, 0x0000);
                oledc_line(&oledc, 13, 19, 13, 16, 0x0000);
            }
            if (pwm_M0 > 30.0f) {
                oledc_line(&oledc, 12, 19, 12, 16, 0xFFFF);
            } else {
                oledc_line(&oledc, 12, 19, 12, 16, 0x0000);
            }
            if (pwm_M0 > 20.0f) {
                oledc_line(&oledc, 11, 19, 11, 16, 0xFFFF);
            } else {
                oledc_line(&oledc, 11, 19, 11, 16, 0x0000);
            }
            if (pwm_M0 > 10.0f) {
                oledc_line(&oledc, 10, 19, 10, 16, 0xFFFF);
            } else {
                oledc_line(&oledc, 10, 19, 10, 16, 0x0000);
            }


            if (pwm_M1 > 90.0f) {
                oledc_line(&oledc, 24+60, 17+60, 24+60, 18+60, 0xFFFF);
                oledc_line(&oledc, 23+60, 16+60, 23+60, 19+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 24+60, 17+60, 24+60, 18+60, 0x0000);
                oledc_line(&oledc, 23+60, 16+60, 23+60, 19+60, 0x0000);
            }
            if (pwm_M1 > 80.0f) {
                oledc_line(&oledc, 22+60, 15+60, 22+60, 20+60, 0xFFFF);
                oledc_line(&oledc, 21+60, 14+60, 21+60, 21+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 22+60, 15+60, 22+60, 20+60, 0x0000);
                oledc_line(&oledc, 21+60, 14+60, 21+60, 21+60, 0x0000);
            }
            if (pwm_M1 > 70.0f) {
                oledc_line(&oledc, 20+60, 13+60, 20+60, 22+60, 0xFFFF);
                oledc_line(&oledc, 19+60, 12+60, 19+60, 23+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 20+60, 13+60, 20+60, 22+60, 0x0000);
                oledc_line(&oledc, 19+60, 12+60, 19+60, 23+60, 0x0000);
            }
            if (pwm_M1 > 60.0f) {
                oledc_line(&oledc, 18+60, 11+60, 18+60, 24+60, 0xFFFF);
                oledc_line(&oledc, 17+60, 19+60, 17+60, 16+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 18+60, 11+60, 18+60, 24+60, 0x0000);
                oledc_line(&oledc, 17+60, 19+60, 17+60, 16+60, 0x0000);
            }
            if (pwm_M1 > 50.0f) {
                oledc_line(&oledc, 16+60, 19+60, 16+60, 16+60, 0xFFFF);
                oledc_line(&oledc, 15+60, 19+60, 15+60, 16+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 16+60, 19+60, 16+60, 16+60, 0x0000);
                oledc_line(&oledc, 15+60, 19+60, 15+60, 16+60, 0x0000);
            }
            if (pwm_M1 > 40.0f) {
                oledc_line(&oledc, 14+60, 19+60, 14+60, 16+60, 0xFFFF);
                oledc_line(&oledc, 13+60, 19+60, 13+60, 16+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 14+60, 19+60, 14+60, 16+60, 0x0000);
                oledc_line(&oledc, 13+60, 19+60, 13+60, 16+60, 0x0000);
            }
            if (pwm_M1 > 30.0f) {
                oledc_line(&oledc, 12+60, 19+60, 12+60, 16+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 12+60, 19+60, 12+60, 16+60, 0x0000);
            }
            if (pwm_M1 > 20.0f) {
                oledc_line(&oledc, 11+60, 19+60, 11+60, 16+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 11+60, 19+60, 11+60, 16+60, 0x0000);
            }
            if (pwm_M1 > 10.0f) {
                oledc_line(&oledc, 10+60, 19+60, 10+60, 16+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 10+60, 19+60, 10+60, 16+60, 0x0000);
            }



            if (pwm_M2 > 90.0f) {
                oledc_line(&oledc, 95-24, 17, 95-24, 18, 0xFFFF);
                oledc_line(&oledc, 95-23, 16, 95-23, 19, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-24, 17, 95-24, 18, 0x0000);
                oledc_line(&oledc, 95-23, 16, 95-23, 19, 0x0000);
            }
            if (pwm_M2 > 80.0f) {
                oledc_line(&oledc, 95-22, 15, 95-22, 20, 0xFFFF);
                oledc_line(&oledc, 95-21, 14, 95-21, 21, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-22, 15, 95-22, 20, 0x0000);
                oledc_line(&oledc, 95-21, 14, 95-21, 21, 0x0000);
            }
            if (pwm_M2 > 70.0f) {
                oledc_line(&oledc, 95-20, 13, 95-20, 22, 0xFFFF);
                oledc_line(&oledc, 95-19, 12, 95-19, 23, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-20, 13, 95-20, 22, 0x0000);
                oledc_line(&oledc, 95-19, 12, 95-19, 23, 0x0000);
            }
            if (pwm_M2 > 60.0f) {
                oledc_line(&oledc, 95-18, 11, 95-18, 24, 0xFFFF);
                oledc_line(&oledc, 95-17, 19, 95-17, 16, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-18, 11, 95-18, 24, 0x0000);
                oledc_line(&oledc, 95-17, 19, 95-17, 16, 0x0000);
            }
            if (pwm_M2 > 50.0f) {
                oledc_line(&oledc, 95-16, 19, 95-16, 16, 0xFFFF);
                oledc_line(&oledc, 95-15, 19, 95-15, 16, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-16, 19, 95-16, 16, 0x0000);
                oledc_line(&oledc, 95-15, 19, 95-15, 16, 0x0000);
            }
            if (pwm_M2 > 40.0f) {
                oledc_line(&oledc, 95-14, 19, 95-14, 16, 0xFFFF);
                oledc_line(&oledc, 95-13, 19, 95-13, 16, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-14, 19, 95-14, 16, 0x0000);
                oledc_line(&oledc, 95-13, 19, 95-13, 16, 0x0000);
            }
            if (pwm_M2 > 30.0f) {
                oledc_line(&oledc, 95-12, 19, 95-12, 16, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-12, 19, 95-12, 16, 0x0000);
            }
            if (pwm_M2 > 20.0f) {
                oledc_line(&oledc, 95-11, 19, 95-11, 16, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-11, 19, 95-11, 16, 0x0000);
            }
            if (pwm_M2 > 10.0f) {
                oledc_line(&oledc, 95-10, 19, 95-10, 16, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-10, 19, 95-10, 16, 0x0000);
            }



            if (pwm_M3 > 90.0f) {
                oledc_line(&oledc, 95-24-60, 17+60, 95-24-60, 18+60, 0xFFFF);
                oledc_line(&oledc, 95-23-60, 16+60, 95-23-60, 19+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-24-60, 17+60, 95-24-60, 18+60, 0x0000);
                oledc_line(&oledc, 95-23-60, 16+60, 95-23-60, 19+60, 0x0000);
            }
            if (pwm_M3 > 80.0f) {
                oledc_line(&oledc, 95-22-60, 15+60, 95-22-60, 20+60, 0xFFFF);
                oledc_line(&oledc, 95-21-60, 14+60, 95-21-60, 21+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-22-60, 15+60, 95-22-60, 20+60, 0x0000);
                oledc_line(&oledc, 95-21-60, 14+60, 95-21-60, 21+60, 0x0000);
            }
            if (pwm_M3 > 70.0f) {
                oledc_line(&oledc, 95-20-60, 13+60, 95-20-60, 22+60, 0xFFFF);
                oledc_line(&oledc, 95-19-60, 12+60, 95-19-60, 23+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-20-60, 13+60, 95-20-60, 22+60, 0x0000);
                oledc_line(&oledc, 95-19-60, 12+60, 95-19-60, 23+60, 0x0000);
            }
            if (pwm_M3 > 60.0f) {
                oledc_line(&oledc, 95-18-60, 11+60, 95-18-60, 24+60, 0xFFFF);
                oledc_line(&oledc, 95-17-60, 19+60, 95-17-60, 16+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-18-60, 11+60, 95-18-60, 24+60, 0x0000);
                oledc_line(&oledc, 95-17-60, 19+60, 95-17-60, 16+60, 0x0000);
            }
            if (pwm_M3 > 50.0f) {
                oledc_line(&oledc, 95-16-60, 19+60, 95-16-60, 16+60, 0xFFFF);
                oledc_line(&oledc, 95-15-60, 19+60, 95-15-60, 16+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-16-60, 19+60, 95-16-60, 16+60, 0x0000);
                oledc_line(&oledc, 95-15-60, 19+60, 95-15-60, 16+60, 0x0000);
            }
            if (pwm_M3 > 40.0f) {
                oledc_line(&oledc, 95-14-60, 19+60, 95-14-60, 16+60, 0xFFFF);
                oledc_line(&oledc, 95-13-60, 19+60, 95-13-60, 16+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-14-60, 19+60, 95-14-60, 16+60, 0x0000);
                oledc_line(&oledc, 95-13-60, 19+60, 95-13-60, 16+60, 0x0000);
            }
            if (pwm_M3 > 30.0f) {
                oledc_line(&oledc, 95-12-60, 19+60, 95-12-60, 16+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-12-60, 19+60, 95-12-60, 16+60, 0x0000);
            }
            if (pwm_M3 > 20.0f) {
                oledc_line(&oledc, 95-11-60, 19+60, 95-11-60, 16+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-11-60, 19+60, 95-11-60, 16+60, 0x0000);
            }
            if (pwm_M3 > 10.0f) {
                oledc_line(&oledc, 95-10-60, 19+60, 95-10-60, 16+60, 0xFFFF);
            } else {
                oledc_line(&oledc, 95-10-60, 19+60, 95-10-60, 16+60, 0x0000);
            }







            control_gtm_flag = FALSE;
        }
    }
#else
    return 0;
#endif
}
