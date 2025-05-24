#include "usbh_helper.h"
#include "CytronMotorDriver.h"

// Configure the motor driver.
CytronMD motorL(PWM_PWM, 8, 9);
CytronMD motorR(PWM_PWM, 14, 15);

volatile uint16_t rotation = 0;
volatile uint16_t direction = 0;
volatile uint16_t throttle = 0;
volatile uint16_t champion = 0;
const int buzzer = 22;

//notes(frequencies in Hz)
// Define notes with their assigned frequency
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247

// Melody for "We Are the Champions" (short snippet)
int championMelody[] = {

  NOTE_D5, NOTE_CS5, NOTE_D5, NOTE_CS5, NOTE_A4, 0, NOTE_FS4, NOTE_B4, NOTE_FS4,

  NOTE_G3, 0, NOTE_A3, 0,

  NOTE_D5, NOTE_E5, NOTE_FS5, NOTE_A5, NOTE_FS5, NOTE_B4, NOTE_CS5, NOTE_B4,

  NOTE_FS3, NOTE_F3, NOTE_E3, NOTE_C4, NOTE_B3,

  NOTE_B4, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_G4, 0, 
  NOTE_G5, NOTE_FS5, NOTE_G5, NOTE_FS5, NOTE_E5, 0, 
  
  NOTE_FS5, 0, NOTE_D5, NOTE_G5, 
  NOTE_FS5, 0, NOTE_D5, NOTE_G5, 
  NOTE_F5, 0, NOTE_D5, NOTE_G5, 
  NOTE_F5, 0, NOTE_D5,

  NOTE_A3, 0, NOTE_A3,

  NOTE_C5, NOTE_A4, NOTE_D5,
};

int championNoteDurations[] = {

  1, 4, 4, 2, 2, 4, 4, 2, 1,

  2, 4, 2, 4,

  1, 4, 4, 2, 2, 4, 4, 1, 
  
  4, 4, 2, 2, 2,

  1, 4, 4, 2, 2, 2, 
  1, 4, 4, 2, 2, 2,

  2, 4, 2, 4,
  2, 4, 2, 4, 
  2, 4, 2, 4, 
  2, 4, 1, 
  
  8, 8, 1,

  4, 8, 1,
};


volatile bool system_ready = false;
unsigned long startup_time;
const unsigned long STARTUP_DELAY = 3000;  // 3 seconds delay

//---------------- Buzzer Functions ----------------//
void playControllerConnectedTone() {
  tone(buzzer, 880, 100);
  delay(100);
  tone(buzzer, 988, 100);
  delay(100);
  noTone(buzzer);
}

void playControllerDisconnectedTone() {
  tone(buzzer, 440, 150);
  delay(150);
  tone(buzzer, 392, 150);
  delay(150);
  noTone(buzzer);
}

void playMelody(int *melody, int *noteDurations, int notesLength) {
  for (int thisNote = 0; thisNote < notesLength; thisNote++) {
    int noteDuration = 800 / noteDurations[thisNote];
    tone(buzzer, melody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzer);
  }
}

void playChampionMelody() {
        playMelody(championMelody, championNoteDurations, 72);
      }


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
  pinMode(buzzer,OUTPUT);

}

void loop() {
  if (!system_ready && (millis() - startup_time >= STARTUP_DELAY)) {
    system_ready = true;
    Serial.println("System Ready!");
    
  }

  if (system_ready) {
    int speed = -mapJoystickToSpeed(throttle);
    speed = applyDeadZone(speed, 5);  // Adjust dead zone as needed

    if (champion == 0x40) {
      playMelody(championMelody, championNoteDurations, 72);
      
      
    }


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
      }  
    } else {
      playControllerDisconnectedTone();
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
void updateControlData(uint16_t new_rotation,  uint16_t new_direction, uint16_t new_throttle, uint16_t championSong) {
  rotation = new_rotation;
  direction = new_direction;
  throttle = new_throttle;
  champion = championSong;
}

extern "C" {
  void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const*, uint16_t) {
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);
    playControllerConnectedTone();
    Serial.printf("HID mounted: addr=%d, inst=%d, VID=%04x, PID=%04x\n", dev_addr, instance, vid, pid);
    if (!tuh_hid_receive_report(dev_addr, instance)) {
      Serial.println("Failed to request HID report");
    }
  }

  void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
    Serial.printf("HID unmounted: addr=%d, inst=%d\n", dev_addr, instance);
    playControllerDisconnectedTone();  // 🔕 Controller disconnected
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
    updateControlData(report[9], report[11], report[1], report[3] );

    if (!tuh_hid_receive_report(dev_addr, instance)) {
      Serial.println("Error requesting next HID report");
    }
  }
}
