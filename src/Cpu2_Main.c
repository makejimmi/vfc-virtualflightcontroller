#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

#include "oledc.h"
#include "tc275_spi_wrap.h"
#include "tc275_gtm_wrap.h"
#include "aurix_pin_mappings.h"

extern IfxCpu_syncEvent cpuSyncEvent;

#if !defined(IFX_CFG_RETURN_FROM_MAIN)
void core2_main (void)
#else
int core2_main (void)
#endif
{
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

        if (gtm_flag)
        {

            gtm_flag = FALSE;
        }
    }
#else
    return 0;
#endif
}
