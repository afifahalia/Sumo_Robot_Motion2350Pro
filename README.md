# Overview

Sumo Robot by using Motion RP2350 Pro as microcontroller and joystick as RC Controller

# Setup

In this project, we are using Robot Battle (500g) - Motion 2350 Pro from Cytron Technologies

## Pin Configuration

| Pin | Details                  |
|-----|--------------------------|
| +   | Power Supply (3.6V - 16V)|
| GND | Power Supply (Ground)    |
| D+  | USB HOST +               |
| D-  | USB HOST -               |
| 8   | Left Motor Forward(M1A)  |
| 9   | Left Motor Backward(M1B) |
| 14  | Right Motor Forward(M4A) |
| 15  | Right Motor Backward(M4B)|

## Workflow
![Sumo Robot drawio 2 drawio (1)](https://github.com/user-attachments/assets/4610c61e-daff-46e2-894c-34433b60239a)

### Mapping

#### Input 
When read input from joysticks, it will gives 20 bytes data which indicate movement for each joysticks
| Input (Bytes) | Variables | Details                                   |
|---------------|-----------|-----------------------------------        |
|   1           | Throttle  | Act as throttle to move sumo robot        |
|   9           | Rotation  | To rotate motor either forward or backward|
|   11          | Direction | Turn sumo robot to the left or right      |

#### Output

After read the different location of each joysticks, we can translate the joysticks movement into motor control logic
| Output | Details                  |
|-----   |--------------------------|
| 0x14   | USB Detected             |
| 0x00   | At Centre(128)           |
| 0x7F   | Maximum (+255)           |
| 0x80   | Minimum (-255)           |

### Workflow

| Output | Details                  |
|-----   |--------------------------|
| 0x14   | USB Detected             |
| 0x00   | At Centre(128)           |
| 0x7F   | Maximum (+255)           |
| 0x80   | Minimum (-255)           |

#### Motor State Based on Mapping

| Joystick (Input) | Throttle | Rotation | Direction | Motor Left Speed  | Motor Right Speed |
| -----------------| -------- | -------- | --------- | ------------------| ----------------- |
| Center           | 0x14     | 0x00     | 0x00      | 0                 | 0                 |
| Upward           | 0x14     | 0x7F     | 0x00      | +255              | +255              |
| Forward Right    | 0x14     | 0x7F     | 0x7F      | +255              | -85               |
| Forward Left     | 0x14     | 0x7F     | 0X80      | -85               | +255              |
| Backward Left    | 0x14     | 0x80     | 0x80      | -85               | +255              |
| Backward Right   | 0x14     | 0x80     | 0x7F      | +255              | -85               |
| Backward         | 0x14     | 0x80     | 0x00      | -255              | -255              |
