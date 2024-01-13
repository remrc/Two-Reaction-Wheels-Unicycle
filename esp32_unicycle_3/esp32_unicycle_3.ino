#include "ESP32.h"
#include <EEPROM.h>
#include <Wire.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32Uni3"); // Bluetooth device name

  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, offsets);
  
  if (offsets.ID == 29) calibrated = true;
    else calibrated = false;
  
  pinMode(BUZZER, OUTPUT);
  pinMode(BRAKE, OUTPUT);
  digitalWrite(BRAKE, HIGH);
  
  pinMode(DIR1, OUTPUT);
  pinMode(ENC1_1, INPUT);
  pinMode(ENC1_2, INPUT);
  attachInterrupt(ENC1_1, ENC1_READ, CHANGE);
  attachInterrupt(ENC1_2, ENC1_READ, CHANGE);
  ledcSetup(PWM1_CH, BASE_FREQ, TIMER_BIT);
  ledcAttachPin(PWM1, PWM1_CH);
  Motor1_control(0);
  
  pinMode(DIR2, OUTPUT);
  pinMode(ENC2_1, INPUT);
  pinMode(ENC2_2, INPUT);
  attachInterrupt(ENC2_1, ENC2_READ, CHANGE);
  attachInterrupt(ENC2_2, ENC2_READ, CHANGE);
  ledcSetup(PWM2_CH, BASE_FREQ, TIMER_BIT);
  ledcAttachPin(PWM2, PWM2_CH);
  Motor2_control(0);
  
  pinMode(DIR3, OUTPUT);
  pinMode(ENC3_1, INPUT);
  pinMode(ENC3_2, INPUT);
  attachInterrupt(ENC3_1, ENC3_READ, CHANGE);
  attachInterrupt(ENC3_2, ENC3_READ, CHANGE);
  ledcSetup(PWM3_CH, BASE_FREQ, TIMER_BIT);
  ledcAttachPin(PWM3, PWM3_CH);
  Motor3_control(0);
  
  delay(2000);
  digitalWrite(BUZZER, HIGH);
  delay(70);
  digitalWrite(BUZZER, LOW);
  angle_setup();
}

void loop() {

  currentT = millis();

  if (currentT - previousT_1 >= loop_time) {
    Tuning();                   // for tuning parameters
    //readControlParameters();  // for remote control
    angle_calc();

    motor1_speed = enc_count1;
    enc_count1 = 0;
    motor2_speed = enc_count2;
    enc_count2 = 0;
    motor3_speed = enc_count3;
    enc_count3 = 0;

    motorY_speed = motor1_speed;
    motorX_speed = -motor2_speed + motor3_speed;
    
    if (vertical && calibrated && !calibrating) {
      digitalWrite(BRAKE, HIGH);
      gyroX = GyX / 131.0;
      gyroY = GyY / 131.0;
      
      gyroYfilt = alphaY * gyroY + (1 - alphaY) * gyroYfilt;
      gyroXfilt = alphaX * gyroX + (1 - alphaX) * gyroXfilt;

      motorY_pos += motorY_speed;
      motorY_pos = constrain(motorY_pos, -320, 320) - speed_remote;
      motorX_pos += motorX_speed;
      motorX_pos = constrain(motorX_pos, -1400, 1400);
      
      int pwm_X = -constrain(K1GainX * robot_angleX + K2GainX * gyroXfilt + K3GainX * motorX_speed + K4GainX * motorX_pos, -255, 255);
      int pwm_Y = constrain(K1GainY * robot_angleY + K2GainY * gyroYfilt + K3GainY * motorY_speed + K4GainY * motorY_pos, -255, 255);

      Motor1_control(pwm_Y);                                       //  Y real wheel
      Motor2_control(pwm_X - steering_remote + rotation);          //  X reaction wheel
      Motor3_control(-pwm_X - steering_remote + rotation);         //  X reaction wheel
    } else {
        Motor1_control(0);
        Motor2_control(0);
        Motor3_control(0);
        digitalWrite(BRAKE, LOW);
        motorY_pos = 0;
        motorX_pos = 0;
      }
    previousT_1 = currentT;
  }
  
  if (currentT - previousT_2 >= 1500) {
      battVoltage((double)analogRead(VBAT) / batt_divider); 
      if (!calibrated && !calibrating) {
      SerialBT.println("first you need to calibrate the balancing point...");
      Serial.println("first you need to calibrate the balancing point... Use Bluetooth!");
    }    
      previousT_2 = currentT;
  }
}
