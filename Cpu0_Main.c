#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

#include <string.h>
#include <tc275_uart_app.h>

IfxCpu_syncEvent cpuSyncEvent = 0;

#if !defined(IFX_CFG_RETURN_FROM_MAIN)
void core0_main(void)
{
#else
int core0_main(void)
#endif
    uint8 message[] = "Hello World\n";
    Ifx_SizeT msg_size = 0;

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
        uart_sendMessage(&message[0], msg_size); /* send entire message */
    }
#else
return 0;
#endif
}
