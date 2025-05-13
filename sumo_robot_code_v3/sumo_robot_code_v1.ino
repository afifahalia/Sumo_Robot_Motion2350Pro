#include "usbh_helper.h"
#include "CytronMotorDriver.h"

// Configure the motor driver.
CytronMD motorL(PWM_PWM, 8, 9);
CytronMD motorR(PWM_PWM, 14, 15);

uint16_t rotation = 0;
uint16_t direction = 0;
uint16_t throttle = 0;

bool system_ready = false;
unsigned long startup_time;
const unsigned long STARTUP_DELAY = 3000;  // 3 seconds delay

int mapJoystickToSpeed(uint16_t value) {
  return map(value, 0, 255, -255, 255);
}

int applyDeadZone(int value, int deadZone) {
  if (abs(value) < deadZone) return 0;
  return value;
}

//---------------- Core 0 ----------------//
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
    int speed = -mapJoystickToSpeed(throttle);
    speed = applyDeadZone(speed, 5);  // Adjust dead zone as needed

    if (throttle == 0x14) {
      //Serial.println ("USB HAS BEEN CONNECTED");
      speed = constrain(speed, -255, 255); //speed range
      if (rotation == 0X7F){           //MOVE FORWARD
        if (direction == 0x00){
            motorL.setSpeed(speed);      //fully forward
            motorR.setSpeed(speed);
            Serial.println("Fully Forward");
          } else if (direction == 0x7F) {  //forward right
            motorL.setSpeed(speed);      
            motorR.setSpeed(-speed/3);
            Serial.println("Turn Right");
          } else if(direction == 0x80) {   //forward left
            motorL.setSpeed(-speed/3);      
            motorR.setSpeed(speed);
            Serial.println("Turn Left");
          } else {
            motorL.setSpeed(0);      
            motorR.setSpeed(0);
            Serial.println("Motor Stopped");
          }
        } else if (rotation == 0x80) {
           if (direction == 0x00){
            motorL.setSpeed(-speed);      //fully backward
            motorR.setSpeed(-speed);
            Serial.println("Fully Backward");
          } else if (direction == 0x7F) { //backward right
            motorL.setSpeed(-speed);      
            motorR.setSpeed(speed/3);
            Serial.println("Turn Back Right");
          } else if (direction == 0x80 ) {                        //backward left
            motorL.setSpeed(speed/3);      
            motorR.setSpeed(-speed);
            Serial.println("Turn Back Left");
          } else {
            motorL.setSpeed(0);      
            motorR.setSpeed(0);
            Serial.println("Motor Stopped");
          }
        } else {
          motorL.setSpeed(0);      //motor stopped
          motorR.setSpeed(0);
          Serial.println("Motor Stopped");
        }
          
    } else {
      //Serial.println("USB ARE NOT CONNECTED");
    }
  }
    Serial.flush();
    delay(10);
}
  

//---------------- Core 1 ----------------//
void setup1() {
  rp2040_configure_pio_usb();
  USBHost.begin(1);
}

void loop1() {
  USBHost.task();
}

//---------------- USB HID Callback ----------------//
void updateControlData(uint16_t new_rotation,  uint16_t new_direction, uint16_t new_throttle) {
  rotation = new_rotation;
  direction = new_direction;
  throttle = new_throttle;
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
    Serial.println("USB has been  Connected = > HID Report: ");
    for (uint16_t i = 0; i < len; i++) {
      if (i != 0){
        Serial.printf("%02X ", report[i]);
      } else {
        Serial.print("USB NOT CONNECTED");
      }
      
    }
    Serial.println();

    // Use Byte 9 as rotation, byte 11 for direction , Byte 1 as to detect joystick existance
    updateControlData(report[9], report[11], report[1] );

    if (!tuh_hid_receive_report(dev_addr, instance)) {
      Serial.println("Error requesting next HID report");
    }
  }
}
