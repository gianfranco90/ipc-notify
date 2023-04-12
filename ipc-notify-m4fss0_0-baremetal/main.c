#include <stdlib.h>
#include "ti_drivers_config.h"
#include "ti_board_config.h"
#include <kernel/dpl/SemaphoreP.h>
#include <kernel/dpl/AddrTranslateP.h>
#include "ipc_notify_benchmark.h"


SemaphoreP_Object exitSem;

int main(void)
{
    System_init();
    Board_init();

    SemaphoreP_constructBinary(&exitSem, 0);
	
    ipc_notify_benchmark_main(NULL);

    SemaphoreP_pend(&exitSem, SystemP_WAIT_FOREVER);

    Board_deinit();
    System_deinit();

    return 0;
}

