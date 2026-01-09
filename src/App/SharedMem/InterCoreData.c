/**
 * \file InterCoreData.c
 * \brief Implementation of Shared Memory storage in LMU.
 */

#include "InterCoreData.h"

/* Global Variables --------------------------------------------------- */

/*
 * g_SharedMem
 *
 * This variable is the physical location of our shared data.
 *
 * CRITICAL MEMORY PLACEMENT:
 * By default, variables go into the local DSPR (Data Scratch Pad RAM) of the
 * core that compiles/links them (usually Core 0). Core 0's DSPR is accessbile
 * by other cores but is slower.
 *
 * LMU (Local Memory Unit) is the dedicated shared RAM for multicore access.
 *
 * The GNU C attribute is being used to ask the linker to place this in the
 * standard LMU section.
 */

#if defined(__GNUC__)
    __attribute__((section(".lmu_global"))) volatile SharedMemory_t g_SharedMem;
#else
    /* Fallback */
    #pragma section ".lmu_global"
    volatile SharedMemory_t g_SharedMem;
    #pragma section
#endif

/* Public API Functions --------------------------------------------------- */

void SharedMem_WriteImuData_C0_C1(ImuData_t imu_data) {
    IfxCpu_acquireMutex((IfxCpu_mutexLock *)&g_SharedMem.mutex_0_1); // acquire lock for core 0 to core 1
    g_SharedMem.data_0_1 = imu_data; // write data to shared memory (struct copy)
    IfxCpu_releaseMutex((IfxCpu_mutexLock *)&g_SharedMem.mutex_0_1); // release lock
}

ImuData_t SharedMem_ReadImuData_C0_C1(void) {
    ImuData_t localCopy;
    IfxCpu_acquireMutex((IfxCpu_mutexLock *)&g_SharedMem.mutex_0_1); // acquire lock for core 0 to core 1
    localCopy = g_SharedMem.data_0_1; // struct copy to local variable for safety
    // for (uint16 i = 0; i < 20000; i++)
    // {
    //     __nop();
    // }
    IfxCpu_releaseMutex((IfxCpu_mutexLock *)&g_SharedMem.mutex_0_1); // release lock
    return localCopy;
}

void SharedMem_WriteOrientation_C1_C2(OrientationData_t data) {
    IfxCpu_acquireMutex((IfxCpu_mutexLock *)&g_SharedMem.mutex_1_2); // acquire lock for core 1 to core 2
    g_SharedMem.data_1_2 = data; // write data to shared memory (struct copy)
    IfxCpu_releaseMutex((IfxCpu_mutexLock *)&g_SharedMem.mutex_1_2); // release lock
}

OrientationData_t SharedMem_ReadOrientation_C1_C2(void) {
    OrientationData_t localCopy;
    IfxCpu_acquireMutex((IfxCpu_mutexLock *)&g_SharedMem.mutex_1_2); // acquire lock for core 1 to core 2
    localCopy = g_SharedMem.data_1_2; // struct copy to local variable for safety
    IfxCpu_releaseMutex((IfxCpu_mutexLock *)&g_SharedMem.mutex_1_2); // release lock
    return localCopy;
}

void SharedMem_WriteImuData_C0_C2(ImuData_t data) {
    IfxCpu_acquireMutex((IfxCpu_mutexLock *)&g_SharedMem.mutex_0_2); // acquire lock for core 0 to core 2
    g_SharedMem.data_0_2 = data; // write data to shared memory (struct copy)
    IfxCpu_releaseMutex((IfxCpu_mutexLock *)&g_SharedMem.mutex_0_2); // release lock
}

ImuData_t SharedMem_ReadImuData_C0_C2(void) {
    ImuData_t localCopy;
    IfxCpu_acquireMutex((IfxCpu_mutexLock *)&g_SharedMem.mutex_0_2); // acquire lock for core 0 to core 2
    localCopy = g_SharedMem.data_0_2; // struct copy to local variable for safety
    IfxCpu_releaseMutex((IfxCpu_mutexLock *)&g_SharedMem.mutex_0_2); // release lock
    return localCopy;
}

