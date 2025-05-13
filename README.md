# Sumo_Robot_Motion2350Pro
Build a competitive 500g class sumo robot from Cytron using a PS2 controller for wireless control and the Motion 2350 Pro board as the main controller

# Overview
| Component           | Details/Notes                                                         |
| ------------------- | --------------------------------------------------------------------- |
| **Microcontroller** | Motion 2350 Pro( Dual-core Arm Cortex-M33 processor)(Arduino use core0|                  
| **Motors**          | 12V 980RPM 2.3kgfcm GM25 High Power Brushed DC Geared Moto            |
| **Power Source**    | Mini LiPo Rechargeable Battery 3S 11.1V 2000mAH                       |
| **Controller**      | PS2 wireless controller + receiver module (core 1)                    |

# Workflow
![Flowchart](https://github.com/user-attachments/assets/5b87a6d4-7cbd-4747-b2f1-e8147ba125fd)

## Mapping 
![ChatGPT Image May 11, 2025, 10_28_49 PM](https://github.com/user-attachments/assets/6a229fc0-8c45-4208-b9ac-332350e1b265)


| Joystick Input          | Action                   | left\_speed | right\_speed |
| ----------------------- | ------------------------ | ----------- | ------------ |
| Centered                | No motion                | 0           | 0            |
| Push forward            | Go forward               | +255        | +255         |
| Pull backward           | Go backward              | -255        | -255         |
| Forward + right         | Curve right while moving | +255        | -85          |
| Forward + left          | Curve left while moving  | -85         | +255         |
| Backward + right        | Curve right while moving | +255        | -85          |
| Backward + left         | Curve left while moving  | -85         | +255         |


# Setup & Configurations
| Pin         | Details/Notes                                                         |
| ------------| --------------------------------------------------------------------- |
| **8**       | Left Motor Forward(Red)                                               |
| **9**       | Left Motor Backward(Black)                                            |
| **14**      | Right Motor Forward(Red)                                              |
| **15**      | Right Motor Backward(Black)                                           |
| **24**      | D+ USB Controller                                                     |
| **25**      | D- USB Controller                                                     |
| **+**       | Power(Red)                                                            |
| **-**       | GND(Black)                                                            |




