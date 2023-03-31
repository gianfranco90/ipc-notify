# IPC Benchmark Application
This repository is a multi-core project for AM64x EVM SoC aimed to benchmark the Inter Processor Communication (IPC) API's provided by the MCU+ SDK for the Real-time cores that are available on the board.

The AM64x device has the following processor subsystems:
- **Arm Cortex-A53 Subsystem (A53SS)**: 1x dual-core CPU (A53SS_CORE0 and A53SS_CORE1)
- **Arm Cortex-R5F Subsystem (R5FSS)**: 2x dual-core CPUs (R5FSS0_0, R5FSS0_1, R5FSS1_0, and R5FSS1_1) 
- **MCU Arm Cortex M4F Subsystem (MCU_M4FSS)**: 1x Arm Cortex-M4F (MCU_M4FSS) 

In this first version a bare-metal application tests the IPC Notify APIs between the 4 cores of the Arm Cortex-R5F subsystem (R4FSS). 
The idea is to measure the round-trip-time of an IPC Notify message from a main core and the oter remote cores. The main core sends the message to a remote core
that echoes back the message to the main core. Measuring the CPU cycles on the main core, between the point the main core sends the mesasge and the moment
in which it receives the response from the remote core, gives the round-trip-time of that message. The half of the round-trip-time is an estimate of the IPC Notify latency.

In particular the R5FSS0_0 core is designated as the MAIN core. Instead the R5FSS0_1, R5FSS1_0 and R5FSS1_1 are the remote cores.
The main core cyclically sends the message at each remote core. 
The number of the message sent at each core can be configured in the code.


## Folder structure:
- **ipc-notify-r5fss0_0-baremetal**: project files, sysconfig file and application main for the core R5FSS0_0
- **ipc-notify-r5fss0_0-baremetal**: project files, sysconfig file and application main for the core R5FSS0_1
- **ipc-notify-r5fss0_0-baremetal**: project files, sysconfig file and application main for the core R5FSS1_0
- **ipc-notify-r5fss0_0-baremetal**: project files, sysconfig file and application main for the core R5FSS1_1
- **ipc-notify-system-four-r5f-baremetal**: system project that acts as collector of the R5F00 project 
- **ipc_notify_benchmark.c/.h**: source files built and linked by all the core projects containing all the logic for the MAIN and REMOTE cores. 

## How to Build on Windows with CCS


## How to build on


## References

