# DER 2024 AMS Firmware v1.0.2

Designed and writen by Cole Bardin (cab572)

Updated: 10/8/2024

## Platform

### Microcontroller

STM32F40G Discovery Board, an ARM Cortex-M4 based microcontroller development board produced by STM.

### Environment

The project can be build and flashed using the STM32CubeIDE. It is an Eclipse based IDE with many added features to improve development on STM boards, such as their IOC file type. This IDE also offers good debugging tools.

[IDE Download Link](https://www.st.com/en/development-tools/stm32cubeide.html)

## Firmware

### Architecture

The entire program is embedded within the `app_data_t` data structure found in `app.h`. This encapsulates all variables, structs, driver interfaces, etc for the program. An instance of `app_data_t` named `app` lives inside the `app.c` file, and a pointer to `app` is be passed to tasks and functions to handle data access. 

All data structures are typedef'd to simplify the code. Also the terms 'data structure' and 'type' will be used interchangeably. In C, a data structure can be typedef'd into a custom data type for convenience.

The `app_data_t` type contains highest priority data such as fault flags, total voltage, min and max cell voltage, max temperature, current, etc. The structure also contains a `board_t` data structure.

The `board_t` type contains representations of physical devices on the ECU board such as the STM32F4, fans, IMD, CANBus transceiver, etc. These devices are represented with their own custom typedef'd data structures.

The `stm32f407g_t` type contain the interfaces for the STM HAL. It holds all the handles, mutexes, and other data types used to interface with the MCU hardware.

Data structures like the `poten_t` and `canbus_t` are wrappers around the physical devices on the board. Instances of these structs are stored in the `board` struct for conventient and reliable access.

Each RTOS task receives a pointer to the `app_data_t` instance so that they can parse out the required interfaces and data.

For example, the CANBus task would require access to the `canbus_t` instance that was initialized during setup. So it would make sense that the task function declares a local `canbus_t *` that points to the instance stored within the shared `app_data_t` instance.

### File Organization

The base firmware is written in C and uses FreeRTOS middleware. It is a rather standard STM32Cube project layout.

Within the `Core` subdirectory, the `Inc` and `Src` directories contain the header and source code files respectively. These two directories have identical internal structures.

Inside of both `Inc` and `Src`, lives the `ext_drivers` and `tasks` subdirectories. Along with `main.c/h`, `app.c/h`, and `board.c/h` files.

The `ext_drivers` subdirectory holds the custom driver files for the MCU, ICs and other devices on the board.

The `tasks` subdirectory contains a file for each task. A task file includes two functions: One function to create the task. And another function for the task body. The task function should be locally scoped to the source file while the task creation function should be included in the header file.

### Program Entry

Like normal C programs, it begins in the `main()` function in `main.c`. After initializing the STM peripherals, the FreeRTOS kernel is also initialized. Then a function called `app_create()` is called. Lastly, the FreeRTOS kernel is started. 

The main function calls `app_create()` located in `app.c`. This function recursively initializes all the nested data structures within the `app_data_t` instance. Lastly, it creates the FreeRTOS tasks to be run.

Once `app_create()` is finished, the kernel is started. After the `osKernelStart()` function, there is an infinite loop trap. This is because `osKernelStart()` should never return. If it does return for some reason, the infinite loop should catch the control and stop `main()` from returning. This behavior is specific to microcontrollers.

## FreeRTOS Tasks

### CANBus Task

The CANBus task creates and sends all outgoing CANBus messages to the transceiver on the AMS. It operates in two modes dictated by the AMS: Discharge and Charge.

In Discharge mode, this task sends messages about the system for the ECU. In the current firmware version, it sends data such as AMS state, AIR state, current sensor reading, IMD OK signal, IMD status, IMD duty cycle, maximum pack temperature, minimum cell voltage, and maximum cell voltage. All floating point values are sent as signed 16-bit fixed point integers.

In Charge mode, the AMS attempts to connect to the charger's CANBus interface. This version uses a the [Elcon HK-J-H440-10](https://www.evwest.com/support/HK-J-H440-10-verA.pdf) charger. The AMS will send the charger the max allowable voltage and current as well as a bit to enable or disable charging. If there is a hardware fail or the charger is overtemp, then charging will be disabled. 

All incomming CANBus messages are handled by the `HAL_CAN_RxFifo0MsgPendingCallback()` interrupt in the `stm32f4xx_it.c` file. This interrupt receives all CANBus messages and determines if it is from the charger. If so, it will update values in the `charger_t` instance. The charger provides it with the voltage and current values it is supplying as well as several error flags.

### Error Task

The Error task monitors all the subsystems and determines if the safety systems must engage. There are hard faults and soft fault. Hard faults are not recoverable and require the `BMS OK` value to be set to false. This will trip Shutdown circuit and open the AIRs. Soft faults are recoverable and do not require action. 

The first thing the task does is it checks if the AIR state has changed. It checks if there is an over current, over/under voltage, over temperature, or charger fault. If so, it will set a 

