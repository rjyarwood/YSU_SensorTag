# New Sensor Guide

This guide will walk you through creating a new sensor on the YSU platform. This guide will cover the software/firmware development. This will not walk through the development of any physical hardware. 

Please consult the main YSU SensorTag README to ensure you have all prerequisites. 

## Developing For a New Board

If the device being written for is a new PCB design, there is some additional work that must be done before moving into Code Composer Studio. Firstly, a new folder should be created for your device in `<path-to-ble-sdk>/examples/`. This is where you will put any projects you develop. Additionally, a new folder will need to be placed in `<path-to-ble-sdk>/src/boards/`. This is where all of your board specific configurations should go. Look at other devices for reference. A final folder should be added in `<path-to-ble-sdk>/src/target/`. This will contain routing information for your board. Again, look at other devices for reference. While in the `target` folder, open `board.h` and `board.c` and add support for your board. DO NOT REMOVE ANYTHING.

## Creating a Project

Go to `ti/simplelink/<ble-sdk>/examples` and find the folder for your device. If it does not exist, go to `Developing For a New Board` above. Choose the closest example as a staring point for your project and copy the entire folder. Do not edit in place or cut. Paste it in your device's folder and name it to your project name. 

Go to Code Composer Studio and open the workspace you would like the project to be in. Select `File -> Import`. Select `'CCS Project'` and browse to where the project you'd like to edit is located. Select the whole project folder. There should be a checkbox showing both an `app` and `stack` project able to import. Select each of these and continue. 


## Adding a Sensor

#### File Setup
After getting your project set up, you can now add support for a new sensor. The first step is to create a folder for your sensor. In this folder I like to have two sub folders, one for direct interaction with the sensor say `'sensor_driver'` and one for BLE. 

```bash
├── <sensorname>
│   ├── ble
│   │   ├── <sensorname>_service.h
│   │   ├── <sensorname>_service.c
│   ├── sensor_driver
│   │   ├── <sensorname>_driver.h
│   │   ├── <sensorname>_driver.c
│   ├── <sensorname>.h
│   ├── <sensorname>.c
│
...
```
This is just an example, it is not required for your sensor to work but can help keep things organized. Once the file structure is set up, you can begin programming for the sensor. 

Note: There are two main options when deciding how to program the sensor. You can define a new task for the sensor which will allow it to handle its own timing and state, or it can be polled from the main task. This makes more sense if the sensor reports on the system as a whole, say a battery state service. For sensors measuring outside variables, I would recommend creating a task. This guide will focus on creating a sensor with its own RTOS task but I will mention where a main thread sensor would be different.

#### File Contents

The `<sensorname>.h` file will contain declarations to four functions.
``` C
extern void <sensorname>_createTask(void);
extern void <sensorname>_init(void);
extern void <sensorname>_processCharChangeEvt(uint8_t paramID);
extern void <sensorname>_reset(void);
```
If your sensor does not use a task,```<sensorname>_createTask``` would be omitted. You can see an example of this in the `SENSOR_EXAMPLE` folder included in the repo. 

`<sensorname>.c` should contain the implementations to these functions as well as some other local functions. This can also be see in the `SENSOR_EXAMPLE` folder.

The `sensor_driver` folder will contain device specific implementation. The only thing that must be included is a function to read the device. This should be declared in the 'h' file so that it can be see by the task.

The `ble` folder will contain everything about the BLE service pertaining to your sensor. There will be an example of this in the `SENSOR_EXAMPLE` folder. 

## Common Fixes
If your project builds but does not seem to be operating correctly, you may be running out of space in your task stack. You can either increase this or edit your code to be a little more memory efficient. This would mean removing any unnecessary local variables, and may mean converting them to static globals so they are not stored in the stack. 

Alternatively, try increasing `ICALL_MAX_NUM_ENTITIES` & `ICALL_MAX_NUM_TASKS` by right clicking on the project, going to `Properties -> Build -> ARM Compiler -> Advanced Options -> Predefined Symbols`. 

## Known Issues
This section will be updated with time. 