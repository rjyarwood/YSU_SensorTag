# YSU_SensorTag
Repository for TI-based boards developed at YSU

## Needed Software

Git: https://gitforwindows.org/

## Installation

There are two main steps. First we need to download all of the TI prerequisites. Then we can set up the Git Repo.

### 1. TI Setup
First, you will need to install Code Composer Studio (CCS) v10x. This will not be
the newest so you will probably need to go to past downloads.

Next, you will need to download the TI BLE SDK version 2.2.1.18.
To do this go to TI's website for the BLE Stack. This is the current
working link: https://www.ti.com/tool/BLE-STACK. You will need to go to all install options and most likely go to 
their archived installers and find 2.2.1 (It should be the only one that
lists the 1350).

The other dependencies should be solvable by CCS itself.

### 2. Git Setup

After installing the BLE Stack, there should be a simplelink folder containing the BLE SDK. This is most likely located at ```C:\ti\simplelink\ble_sdk_2_02_01_18```.
Open your preferred terminal in this location. 

```
git init
git remote add origin https://github.com/rjyarwood/YSU_SensorTag.git
git checkout BLE
git pull
```

To check if this worked, go into ```examples``` and see if there is a ```ysu1stk``` folder. 

## Opening an Example

Open Code Composer Studio and create a workspace. Go to ```File->Open Projects from File System...``` Then navigate to ```C:\ti\simplelink\ble_sdk_2_02_01_18\examples```
choose the appropriate board and example and then choose select the ```ccs``` folder. Both ```app``` and ```stack``` are needed for any BLE applications. You can either open them spererately or
just open the ```ccs``` folder and check each one.

## Documentation

This is in progress. I hope to have it hosted on this repo

## Adding a board

This will be covered in the Wiki
