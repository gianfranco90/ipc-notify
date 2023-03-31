
#include "ipc_notify_benchmark.h"
#include <stdio.h>
#include <kernel/dpl/ClockP.h>
#include <kernel/dpl/CycleCounterP.h>
#include <kernel/dpl/SemaphoreP.h>
#include <drivers/ipc_notify.h>
#include "ti_drivers_open_close.h"
#include "ti_board_open_close.h"
#include <drivers/gpio.h>

/* This module implements several IPC message exchanges among R5 cores
 *
 * One of the core is designated as the 'main' core
 * and other cores are desginated as `remote` cores.
 * The purpose is to measure the latency of the IPC notify between two different R5 cores.
 * We measure the round-trip time of a message of 32-bit that goes from
 * the MAIN core to the REMOTE one and returns back.
 * The REMOTE core sends the message back as soon as it receives it (t2-t1~=0).
 */

/* global variables declared as extern in ipc_notify_benchmark.h, they need to be defined */
uint32_t gGpioBaseAddrTranslated;
uint32_t gGpioPinNum;
uint32_t gGpioDirection;

/* number of iterations of message exchange to do */
uint32_t gMsgsCount = 1u;
/* client ID that is used to send and receive messages */
uint32_t gClientId = 4u;

Float64 gCpuFreq = 0.0f;

const uint32_t gPulseWidth = 100u;

/* main core that starts the message exchange */
uint32_t gMainCoreId = CSL_CORE_ID_R5FSS0_0;
/* remote cores that echo messages from main core, make sure to NOT list main core in this list */
uint32_t gRemoteCoreId[] = {
    CSL_CORE_ID_R5FSS0_1,
    CSL_CORE_ID_R5FSS1_0,
    CSL_CORE_ID_R5FSS1_1,
    CSL_CORE_ID_MAX /* this value indicates the end of the array */
};

/* semaphore's used to indicate a main core has finished all message exchanges */
SemaphoreP_Object gReplyReceivedSem;

uint32_t getCpuCycles(uint32_t before, uint32_t after)
{
    uint32_t cpuCycles = 0;
    if(after > before)
    {
        cpuCycles = after - before;
    }
    else
    {
        cpuCycles = (0xFFFFFFFFU - before) + after;
    }
    return cpuCycles;
}

void compute_and_print_cpu_freq(uint32_t coreId)
{
    CycleCounterP_reset();

    uint64_t curTime = ClockP_getTimeUsec();
    uint32_t cycleCount = CycleCounterP_getCount32();

    ClockP_usleep(1 * 1000); //sleep for 1ms

    cycleCount = CycleCounterP_getCount32() - cycleCount; /*no overlflow dangere here for such a short duration*/

    curTime = ClockP_getTimeUsec() - curTime; /*get time and calculate diff, ClockP return 64-bit value so no overlflow here*/

    gCpuFreq = cycleCount / (Float64) curTime;

    DebugP_log("[CORE %d] - Measured time = %d usecs, CPU cycles = %d, CPU freq = %.2f MHz\r\n", coreId, (uint32_t)curTime, cycleCount, (Float32) gCpuFreq);

}

void gpio_reset(uint32_t baseAddrTranslated, uint32_t pinNum, uint32_t direction)
{
	GPIO_setDirMode(baseAddrTranslated, pinNum, direction);
	GPIO_pinWriteLow(baseAddrTranslated, pinNum);	
}

void gpio_pulse(uint32_t baseAddrTranslated, uint32_t pinNum, const uint32_t pulseWidthUsec)
{
	GPIO_pinWriteHigh(baseAddrTranslated, pinNum);
	ClockP_usleep(pulseWidthUsec);
	GPIO_pinWriteLow(baseAddrTranslated, pinNum);
}
void ipc_notify_msg_handler_main_core(uint32_t remoteCoreId, uint16_t localClientId, uint32_t msgValue, void *args)
{
    SemaphoreP_post(&gReplyReceivedSem);
}

void ipc_notify_benchmark_main_core_start(void)
{
    int32_t status = -1;
    uint32_t i = 0;
    uint32_t j = 0;

    uint32_t t0_cpuCycles = 0;
    uint32_t t3_cpuCycles = 0;
    uint32_t cpuCycles_t3_t0 = 0;

    uint32_t cpuCyclesAcc_t3_t0 = 0;

    Float32 cpuCyclesAvg_t3_t0 = 0.0;

    SemaphoreP_constructBinary(&gReplyReceivedSem, 0);
    /* register a handler to receive messages */
    status = IpcNotify_registerClient(gClientId, ipc_notify_msg_handler_main_core, NULL);
    DebugP_assert(status==SystemP_SUCCESS);

    /* wait for all cores to be ready */
    IpcNotify_syncAll(SystemP_WAIT_FOREVER);

    DebugP_log("[IPC NOTIFY] Message exchange started by main core !!!\r\n");

    for(i=0; gRemoteCoreId[i]!=CSL_CORE_ID_MAX; i++)
    {
        CycleCounterP_reset();
        for(j=0; j < gMsgsCount; j++)
        {
            uint32_t msgValue = 0;
			
            /* send message's to i-th remote core wait for message to be put in HW FIFO */
			t0_cpuCycles = CycleCounterP_getCount32();
			//gpio_pulse(gGpioBaseAddrTranslated, gGpioPinNum, gPulseWidth);
            status = IpcNotify_sendMsg(gRemoteCoreId[i], gClientId, msgValue, 1);
            //DebugP_assert(status==SystemP_SUCCESS);
            
			/* wait i-th remote core to reply to the message*/
            SemaphoreP_pend(&gReplyReceivedSem, SystemP_WAIT_FOREVER);
			t3_cpuCycles = CycleCounterP_getCount32();
			//gpio_pulse(gGpioBaseAddrTranslated, gGpioPinNum, gPulseWidth);


            cpuCycles_t3_t0 = getCpuCycles(t0_cpuCycles, t3_cpuCycles);
            cpuCyclesAcc_t3_t0 += cpuCycles_t3_t0;

        }

        cpuCyclesAvg_t3_t0 = cpuCyclesAcc_t3_t0 / (Float32) gMsgsCount;

        DebugP_log("\r\n[IPC NOTIFY] Results of exchanging %u messages between Main core %d and Remote core %d\r\n", gMsgsCount, gMainCoreId, gRemoteCoreId[i]);
		DebugP_log("[IPC NOTIFY] Main core CPU frequency: %.2f MHz\r\n", gCpuFreq);
        DebugP_log("[IPC NOTIFY] Average round-trip delay CPU cycles: %.3f (time = %.3f usecs)\r\n", cpuCyclesAvg_t3_t0, (Float32)(cpuCyclesAvg_t3_t0 * (1 / (Float64) gCpuFreq)));

    }
}

void ipc_notify_msg_handler_remote_core(uint32_t remoteCoreId, uint16_t localClientId, uint32_t msgValue, void *args)
{
    /* on remote core, we have registered handler on the same client ID and current core client ID */
	//gpio_pulse(gGpioBaseAddrTranslated, gGpioPinNum, gPulseWidth);
    IpcNotify_sendMsg(remoteCoreId, localClientId, msgValue, 1);
}

void ipc_notify_benchmark_remote_core_start(void)
{
    int32_t status;

    /* register a handler to receive messages */
    status = IpcNotify_registerClient(gClientId, ipc_notify_msg_handler_remote_core, NULL);
    DebugP_assert(status==SystemP_SUCCESS);

    /* wait for all cores to be ready */
    IpcNotify_syncAll(SystemP_WAIT_FOREVER);

}

void ipc_notify_benchmark_main(void *args)
{
    Drivers_open();
    Board_driversOpen();

    compute_and_print_cpu_freq(IpcNotify_getSelfCoreId());
	
	//gpio_reset(gGpioBaseAddrTranslated, gGpioPinNum, gGpioDirection);
	
    if(IpcNotify_getSelfCoreId()==gMainCoreId)
    {
        ipc_notify_benchmark_main_core_start();
    }
    else
    {
        ipc_notify_benchmark_remote_core_start();
    }

    Board_driversClose();
    /* We dont close drivers to let the UART driver remain open and flush any pending messages to console */
    /* Drivers_close(); */
}
