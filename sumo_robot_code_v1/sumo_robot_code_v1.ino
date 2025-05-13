/*  Sumo Robot using Motion RP2350 Pro

DESCRIPTION:
This example code will read usb device report on the MOTION 2350 Pro using Arduino(C/C#)

REFERENCE:
2019 Ha Thach for Adafruit Industries
2024 Cytron Technoogies Cytron-MOTION-2350-PRO


/* This example demonstrates use of both device and host, where
 * - Device run on native usb controller (roothub port0)
 * - Host depending on MCUs run on either:
 *   - rp2040: bit-banging 2 GPIOs with the help of Pico-PIO-USB library (roothub port1)
 *   - samd21/51, nrf52840, esp32: using MAX3421e controller (host shield)
 *
 * Requirements:
 * - For rp2040:
 *   - [Pico-PIO-USB](https://github.com/sekigon-gonnoc/Pico-PIO-USB) library
 *   - 2 consecutive GPIOs: D+ is defined by PIN_USB_HOST_DP, D- = D+ +1
 *   - Provide VBus (5v) and GND for peripheral
 *   - CPU Speed must be either 120 or 240 Mhz. Selected via "Menu -> CPU Speed"
 * - For samd21/51, nrf52840, esp32:
 *   - Additional MAX2341e USB Host shield or featherwing is required
 *   - SPI instance, CS pin, INT pin are correctly configured in usbh_helper.h
 */

/* Host example will get device descriptors of attached devices and print it out via
 * device cdc (Serial) as follows:
 *    Device 1: ID 046d:c52f
      Device Descriptor:
        bLength             18
        bDescriptorType     1
        bcdUSB              0200
        bDeviceClass        0
        bDeviceSubClass     0
        bDeviceProtocol     0
        bMaxPacketSize0     8
        idVendor            0x046d
        idProduct           0xc52f
        bcdDevice           2200
        iManufacturer       1     Logitech
        iProduct            2     USB Receiver
        iSerialNumber       0
        bNumConfigurations  1
 *
 */

//Library used

//Usb host library(Pico PIO USB by sekigon-gonnoc) 
#include "usbh_helper.h"

//Motor driver library(Cytron Motor Driver by Cytron Technologies)
#include "CytronMotorDriver.h"

// Motor pin configurations
CytronMD motorL(PWM_PWM, 8, 9);
CytronMD motorR(PWM_PWM, 14, 15);

//initial input declaration 
uint16_t input_1 = 0;
uint16_t input_2 = 0;
uint16_t input_3 = 0;
uint16_t motorSpeed = 0;

//startup 
bool system_ready = false;
unsigned long startup_time;
const unsigned long STARTUP_DELAY = 3000;  // 3 seconds delay before system starts

//declaration motor value will system read
int mapJoystickToSpeed(uint16_t value) {
  return map(value, 0, 255, -255, 255);
}

//ignore small unintentional movement during using joystick
int applyDeadZone(int value, int deadZone) {
  if (abs(value) < deadZone) return 0;
  return value;
}

//---------------- ----Core 0 -------------------//
//---------------- Arduino Logic----------------//

//Initial setup, motor are not moving, need to set baud rate 115200 to read output from joysticks
void setup() {
  Serial.begin(115200);
  Serial.println("Motor System Starting...");
  motorL.setSpeed(0);
  motorR.setSpeed(0);
  startup_time = millis();
}

void loop() {
  if (!system_ready && (millis() - startup_time >= STARTUP_DELAY)) {
    system_ready = true;
    Serial.println("System Ready!");
  }

  if (system_ready) {
    int speed = -mapJoystickToSpeed(motorSpeed);
    speed = applyDeadZone(speed, 5);  // Adjust dead zone as needed

      if (motorSpeed != 0x14){
        Serial.print("Host USB connected");

        if (input_1 == 0xFF) {  // forward button press
          if (input_3 == 0xFF) {
            speed = constrain(speed, -255, 255); //forward right
            motorL.setSpeed(speed);
            motorR.setSpeed(-speed/3);
            } else if (input_3 == 0x80) {
            speed = constrain(speed, -255, 255); //forward left
            motorL.setSpeed(-speed/3);
            motorR.setSpeed(speed);
            } else {
            speed = constrain(speed, -255, 255); //full forward
            motorL.setSpeed(speed);
            motorR.setSpeed(speed);
            }
          } else if (input_1 == 0x80) {
          if (input_3 == 0xFF){
            speed = constrain(speed, -255, 255); //backward right
            motorL.setSpeed(speed);
            motorR.setSpeed(-speed/3);
            } else if (input_3 == 0x80){
            speed = constrain(speed, -255, 255); //backward left
            motorL.setSpeed(-speed/3);
            motorR.setSpeed(speed);
            } else {
            speed = constrain(speed, -255, 255); //full backward
            motorL.setSpeed(-speed);
            motorR.setSpeed(-speed);
            }
          } else {
          motorL.setSpeed(0); //both motor stopped moving
          motorR.setSpeed(0);
          speed = 0;
          } 
          else {
          Serial.print("Device not connected, Please check your USB or reset your board");
          }
      
    }

  Serial.flush();
  delay(10);
  }
}
//---------------- ------Core 1--------- ----------------//
//---------------- PS2 controller logic ----------------//
void setup1() {
  rp2040_configure_pio_usb();
  USBHost.begin(1);
}

void loop1() {
  USBHost.task();
}

//---------------- USB HID Callback ----------------//
void updateControlData(uint16_t new_input_1,  uint16_t new_input_2, uint16_t new_input_3, uint16_t new_motorSpeed) {
  input_1 = new_input_1; //to read byte 9 (left joystick for up/down)
  input_2 = new_input_2; //to read byte 13 (right joystick for up/dowm)
  input_3 =new_input_3;  //to read byte 11 (right joystick for right/left)
  motorSpeed = new_motorSpeed;
}

extern "C" {
  void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const*, uint16_t) {
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);
    Serial.printf("HID mounted: addr=%d, inst=%d, VID=%04x, PID=%04x\n", dev_addr, instance, vid, pid);
    if (!tuh_hid_receive_report(dev_addr, instance)) {
      Serial.println("Failed to request HID report");
    }
  }

  void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
    Serial.printf("HID unmounted: addr=%d, inst=%d\n", dev_addr, instance);
  }

  void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const *report, uint16_t len) {
    // Debug raw data
    Serial.print("Device connected => HID Report: ");
    for (uint16_t i = 0; i < len; i++) {
      Serial.printf("%02X ", report[i]);
    }
    Serial.println();

    // Use Byte 9,13,11 as button input, Byte 4 as motor speed input
    updateControlData(report[8], report[13],report[11], report[1] );

    if (!tuh_hid_receive_report(dev_addr, instance)) {
      Serial.println("Error requesting next HID report");
    }
  }
}
