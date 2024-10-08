# DER 2024 AMS Firmware v1.0.2

Designed and writen by Cole Bardin (cab572)

Updated: 10/8/2024

## Platform

### Microcontroller

[STM32F40G Discovery](https://www.st.com/en/evaluation-tools/stm32f4discovery.html) Board, an ARM Cortex-M4 based microcontroller development board produced by STM.

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

The Error task monitors all the subsystems and determines if the safety systems must engage. There are hard faults and soft fault. Hard faults are not recoverable and require the `BMS_OK` value to be set to false. This will trip Shutdown circuit and open the AIRs. Soft faults are recoverable and do not require action. 

The first thing the task does is it checks if the AIR state has changed. If the AIR state was HIGH then turned LOW, the AMS will intentionally trip the shutdown circuit.

Then it checks for Hard faults. It checks if there is an over current, over/under voltage, over temperature, or charger fault. If there is a hard fault, it will set `BMS_OK` to false.

It also checks for soft faults. Soft faults include CANBus fault, current sensor fault, fan fault, CLI fault. 

There is one more thing to note. Due to the nature of the LTC6813 segment communication, it does not immediately populate all the values. It takes a few cycles to get all the segment voltages and temperatures. Thus on startup, the full error testing waits until the initial LTC fault is cleared before being able to set hard faults. This is because hard faults are defined as over/under voltage and over temperature. These statistics are sourced from the LTC segment monitoring. So the Error task waits until there is good communication with the segments before performing hard fault detection.

### CLI Task (Command Line Interface)

The CLI Task handles all incomming commands from the user. When connected to the serial port, each character received triggers the `HAL_UART_RxCpltCallback()` interrupt in `stm32f7xx_it.c`. The characters are stored in a buffer. Once a newline has been sent, the interrupt callback notifies the CLI task to process the command. 

Once notified, the CLI task copies the buffer and tokenizes it. Once broken into tokens, the command can be dynamically processed comparing them to the entries in the `command_t` array.

### Current Sensor Task

The Current sensor task reads the analog voltages created by the Hall-Effect current sensor. It is a simple task. The sensor used is the [DHAB S/155](https://www.lem.com/en/product-list/dhab-s155). It has 2 channels, a high and a low channel. The low channel measures from -50A to 50A, the high channel measures -800A to 800A. The task reads both channels and then determines what the current output is with a simple algorithm. If the low channel measures above 50A or below -50A, it uses the high channel output, otherwise it uses the low channel.

### Fan Task

The Fan task monitors the maximum temperatures reported and will control the fans accordingly. It has a High threshold and a Low threashold. If the maximum temperature is above the high threshold, the fans turn on at 100%. Once the max temperature is below the Low threashold, the fans turn off. The Low threashold is below the High threshold as to not have the fans continually turn on and off. The idea is to lower the temperature a good amount below the High threshold.

### IMD Task (Insulation Monitoring Device)

The IMD task reads from the Insulation Monitoring Device sensor. The sensor used is dictated by the FSAE EV rules and is the [Bender ISOMETER IR155-3204](https://www.bender.de/en/products/insulation-monitoring/isometer-ir155-3203-ir155-3204/). 

This sensor has a few outputs. It has a 10V digital signal that relays the general error status. When it is 10V, the sensor has not detected a fault. It will switch to GND when a fault occurs. There is also another output from the sensor. There is a PWM pin. When an error occurs, the PWM duty cycle and frequency can dictate what the error was. But in all cases, if the `OK_HS` is low, there is an error and shutdown must be tripped.

The actual task only reads the OK signal. While the PWM values are read from the `HAL_TIM_IC_CaptureCallback()` interrupt within `stm32f4xx_it.c`. This is because during normal operation, the sensor generates a PWM signal at 10Hz from 5~95% duty cycle during normal operation. If for some reason the PWM interrupt did not trigger, the OK status can still be read by the task.

### LTC Task

The LTC task communicates with the [LTC6813](https://www.analog.com/media/en/technical-documentation/data-sheets/LTC6813-1.pdf) ICs on top of each segment via isoSPI. This is a daisy chained protocol, so it just sends 5 messages and expects 5 responses at a time. It reads the voltages and temperatures from the LTCs, then it checks to see if there needs to be cell balancing.

The voltage reading is pretty straight forward. There is a command for the LTC6813 called ADCV which reads all the cell voltages. The values are returned by the ICs and autopopulated into the data structure. Then the values are converted and the min and max are calculated.

Temperature reading is a bit more complicated. There is no built in temperature monitoring available in the LTC. Instead we use 24 NTCs on the Battery Management Bords (BMBs) that go into 3 8 to 1 analog multiplexors. The multiplexor outputs are read as GPIO analog inputs on the LTC chips. So there are 3 out of the 9 GPIOs are used to read temperatures. Two more of the GPIOs are used as an I2C controller. The analog multiplexors are driven by I2C. So the AMS can tell the LTC chips to send arbitrary I2C signals. Temperature reading goes as following: select channel mux, poll GPIO pins with ADCs, read auxiliary registers from LTC (the actual voltages read by the GPIO pins), convert to temperatures, determine next channel to read.

To perform cell balancing, every task iteration, the AMS checks to see if it is in balancing mode. If so, it determines which cells need to be balanced and sets the discharge bits accordingly. On each iteration, it clears the discharge bits for all cells so that once a cell is done balancing, it will stop discharging. If no cells are found that need balancing, it will exit balance mode.

