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
![Sumo Robot](https://github.com/user-attachments/assets/1ed11242-cd27-499b-b096-7be169128817)


# Configuration

Since we are using Motion 2350 Pro which have dual core processor, we can separate usb and arduino logic process. To translate PS2 Joysticks controller, we need to identify each button configuration. Based on the configuration, we can set motor output which shows in serial output.

| Action   | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    | 9    | 10   | 11   | 12   | 13   | 14   | 15   | 16   | 17   | 18   | 19   | 20   |
| -------- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| Centered | 0x00 | 0x14 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 |
| Forward  | 0x00 | 0x14 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | **0xFF**| **0x7F** | 0x00 | **0xFF**| **0x7F** | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 |
| Backward | 0x00 | 0x14 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | **0x80**| 0x00 | 0x00 | 0x00 | 0x00 |**0x80**| 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 |
| Right(F) | 0x00 | 0x14 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | **0xFF** | **0x7F** | **0xFF** | **0x7F** | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 |
| Left(F)  | 0x00 | 0x14 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | **0xFF** | **0x7F** | 0x00 | **0x80**| 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 |
| Right(B) | 0x00 | 0x14 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | **0x80** | 0x00 | **0xFF**| **0x7F** | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 |
| Left(B)  | 0x00 | 0x14 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | **0x80**| 0x00| 0x00 | **0x80** |0x00| 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 | 0x00 |

### Indicator of Mapping
| Output(Bytes)         | Details/Notes                                                         |
| ----------------------| --------------------------------------------------------------------- |
| **0xFF**              | Push Max (+255)                                                       |
| **0x00**              | Center (180)                                                          |
| **0x80**              | At Minimum(-255)                                                      |

Based on this indicator, we can assign direction and speed based on input. 

If pin 8 = ff, direction forward
	If pin 11 = FF, direction is right
	Else direction is left
Else if pin 8 = 80, direction backward
	If pin = 11, direction is right
	Else direction is left
Else motor stop moving


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


## Setup
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




