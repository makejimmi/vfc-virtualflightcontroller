#ifndef APP_SHAREDMEM_INTERCOREDATA_H_
#define APP_SHAREDMEM_INTERCOREDATA_H_

#include "Ifx_Types.h"
#include "IfxCpu.h"

/**
 * \brief Data produced by Core 0 (IMU).
 * Units:
 *  - Accel: g
 *  - Gyro:  deg/s
 */
typedef struct {
    float accel_x;
    float accel_y;
    float accel_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
} ImuData_t;

/**
 * \brief Data produced by Core 1 (Orientation/Attitude).
 * Units: Degrees
 */
typedef struct {
    float roll;
    float pitch;
    float yaw;
} OrientationData_t;

/**
 * \brief The main container for Inter-Core Communication.
 *
 * This struct must be placed in LMU to be accessible by all cores.
 * Access Rules:
 *  - 1. Acquire appropriate Mutex.
 *  - 2. Read/Write Data.
 *  - 3. Release Mutex.
 */
typedef struct {
    // comm: core 0 (prod) -> core 1 (cons)
    IfxCpu_mutexLock mutex_0_1;
    ImuData_t        data_0_1;

    // comm: core 1 (prod) -> core 2 (cons)
    IfxCpu_mutexLock mutex_1_2;
    OrientationData_t data_1_2;

    // potentially useless (nice to have? use for e.g. debug data)
    // comm: core 0 (prod) -> core 2 (cons  )
    IfxCpu_mutexLock mutex_0_2;
    ImuData_t        data_0_2;

} SharedMemory_t;

/* Global Variables --------------------------------------------------- */

/*
 * Global handle to the shared memory.
 * Defined in InterCoreData.c
 */
extern volatile SharedMemory_t g_SharedMem;

/* Public API Functions --------------------------------------------------- */

/**
 * \brief Core 0 calls this to publish IMU data for Core 1.
 * \param data The calculated IMU data struct.
 */
void SharedMem_WriteImuData_C0_C1(ImuData_t data);

/**
 * \brief Core 1 calls this to read the latest IMU data from Core 0.
 * \return Copy of the safe IMU data.
 */
ImuData_t SharedMem_ReadImuData_C0_C1(void);

/**
 * \brief Core 1 calls this to publish Angles for Core 2.
 * \param data The calculated Orientation struct.
 */
void SharedMem_WriteOrientation_C1_C2(OrientationData_t data);

/**
 * \brief Core 2 calls this to read the latest Angles from Core 1.
 * \return Copy of the safe Orientation data.
 */
OrientationData_t SharedMem_ReadOrientation_C1_C2(void);

/**
 * \brief Core 0 calls this to publish IMU data for Core 2 (e.g. for Debug UART).
 * \param data The calculated IMU data struct.
 */
void SharedMem_WriteImuData_C0_C2(ImuData_t data);

/**
 * \brief Core 2 calls this to read the latest IMU data from Core 0.
 * \return Copy of the safe IMU data.
 */
ImuData_t SharedMem_ReadImuData_C0_C2(void);


#endif /* APP_SHAREDMEM_INTERCOREDATA_H_ */
