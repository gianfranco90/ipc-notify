#include <stdlib.h>
#include "ti_drivers_config.h"
#include "ti_board_config.h"
#include <kernel/dpl/SemaphoreP.h>
#include <kernel/dpl/AddrTranslateP.h>
#include "ipc_notify_benchmark.h"

/* global variables declared as extern in ipc_notify_benchmark.h, they need to be defined */
uint32_t gGpioBaseAddrTranslated;
uint32_t gGpioPinNum;
uint32_t gGpioDirection;

SemaphoreP_Object exitSem;

int main(void)
{
    System_init();
    Board_init();

    SemaphoreP_constructBinary(&exitSem, 0);
	
	gGpioBaseAddrTranslated = (uint32_t) AddrTranslateP_getLocalAddr(GPIO0_R5FSS0_1_BASE_ADDR);
	gGpioPinNum = GPIO0_R5FSS0_1_PIN;
	gGpioDirection = GPIO0_R5FSS0_1_DIR;
	
    ipc_notify_benchmark_main(NULL);

    SemaphoreP_pend(&exitSem, SystemP_WAIT_FOREVER);

    Board_deinit();
    System_deinit();

    return 0;
}
