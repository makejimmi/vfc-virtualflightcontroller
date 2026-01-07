#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

#include <tc275_led_app.h>

extern IfxCpu_syncEvent cpuSyncEvent;

#if !defined(IFX_CFG_RETURN_FROM_MAIN)
void core1_main (void)
{
#else
	int core1_main (void)
#endif
    IfxCpu_enableInterrupts();
    /*
     * !!WATCHDOG1 IS DISABLED HERE!!
     * Enable the watchdog in the demo if it is required and also service the watchdog periodically
     * */
    IfxScuWdt_disableCpuWatchdog (IfxScuWdt_getCpuWatchdogPassword ());

    /* Cpu sync event wait*/
    IfxCpu_emitEvent(&cpuSyncEvent);
    IfxCpu_waitEvent(&cpuSyncEvent, 1);

    initLED();  /* Initialize the LED port pin      */

#if !defined(IFX_CFG_RETURN_FROM_MAIN)
    while (1)
    {
        blinkLED(); /* Make the LED blink           */
    }
#else
    return 0;
#endif
}
