This is the repository of the multicore project of course `Embedded Systems Software 1` of the master course `Embedded Systems` by Benjamin Pal, Mathias X., Thomas Klima and Jimmi Bacher.

# Project Organization

```text
multicore_project/
├── .ads/                    # ADS Metadata
├── .settings/               # Eclipse Settings
├── Configurations/          # AURIX Configuration (Ifx_Cfg.h, linker scripts)
├── Libraries/               # Infineon Low Level Drivers (iLLD) - DO NOT MOVE THIS
├── src/
│   ├── Cpu0_Main.c
│   ├── Cpu1_Main.c
│   └── Cpu2_Main.c
│   ├── App/                 # Application Logic
│   │   ├── imu_sensor/
│   │   │   ├── imu_sensor.c
│   │   │   ├── imu_sensor.h
│   │   ├── roll_pitch_yaw/
│   │   │   ├── roll_pitch_yaw.c
│   │   │   ├── roll_pitch_yaw.h
│   │   └── .../
├── .cproject
├── .project
└── README.md
```

**Folder Breakdown:**

*   **src/**: This is the workspace.
    *   **Main/**: `Cpu*_Main.c` entry points here.
    *   **App/**: Pure application logic (like PID controllers).
    *   **Drivers/**: Custom wrappers for hardware peripherals.
*   **Libraries/**: Standard Infineon drivers (iLLD). Don't touch or move this folder.
*   **Configurations/**: Linker scripts and global config headers.

# Project Task: Virtual Flight Controller

## Goal
Create a virtual quadcopter flight controller using the AURIX TC275SB hardware.

## Team & Roles

| Member | Role | Responsibilities |
|---|---|---|
| **Mathias** | Core 0 | **IMU Data Acquisition**: Measure board orientation via I2C/SPI, scale data to physical units, write to Shared Mem. |
| **Benjamin** | Core 1 | **Angle Estimation**: Read Shared Mem, calculate Roll/Pitch/Yaw (Complementary Filter), write to Shared Mem. |
| **Thomas** | Core 2 | **Controller & UI**: UART output to visualizer, P-Controller for motor RPMs, OLED Display output. |
| **Jimmi** | Memory Manager | **Shared Memory & IPC**: Implementation of protected shared memories (Mutex) and synchronization logic. |

## Detailed Function Description

**Core 0** uses the IMU ClickBoard to measure the board’s orientation. It communicates with the IMU ClickBoard via I2C or SPI and sends forth the measured data that has been scaled to physical units (g, °/sec) via shared memory.

**Core 1** receives this data and uses them to estimate Roll/Pitch/Yaw angles. It sends these angles forth via shared memory.

**Core 2** receives these angles and sends them out via UART in a format compatible with the IMU visualizer. Furthermore, it implements a simple P controller that determines the RPM of the four motors to bring a hypothetical quadcopter into level flight. Finally, it displays the RPMs on an OLED C click with which it communicates via SPI.

**Shared Memory**
Core 0 and Core 1 can exchange data via a shared memory (SM) as can Core 0 and Core 2. The shared memories must be protected. The core that uses the shared memory must claim ownership before reading or writing the shared memory and release it. Use a mutex to protect the memories.

## Hardware Setup
*   **Board**: Aurix TC275SB with ClickShield
*   **IMU**: 13DOF Click / 6DOF IMU 14 Click / BMI088 (Use CON 1 for I2C, CON 2 for SPI)
*   **Display**: OLED C Click

## Relevant Resources
*   [Complementary Filter](https://ahrs.readthedocs.io/en/latest/filters/complementary.html)
*   [IMU Visualizer](https://embsys.technikum-wien.at/projects/drohnfh/imu-visualizer/imuvisualizer.php)
*   [Proportional Control](https://en.wikipedia.org/wiki/Proportional_control)
*   [Quad Copter Dynamics](https://cookierobotics.com/066/)
