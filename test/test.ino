#include <Wire.h>

#define BUZZER      27
#define VBAT        34

#define BRAKE       26

#define DIR1        4
#define ENC1_1      35
#define ENC1_2      33
#define PWM1        32
#define PWM1_CH     1

#define DIR2        15
#define ENC2_1      13
#define ENC2_2      14
#define PWM2        25
#define PWM2_CH     0

#define DIR3        5
#define ENC3_1      16
#define ENC3_2      17
#define PWM3        18
#define PWM3_CH     2

#define TIMER_BIT  8
#define BASE_FREQ  20000

#define MPUaddr    0x68

volatile int  enc_count1 = 0, enc_count2 = 0, enc_count3 = 0;

int motor1_counter = 0;
int16_t motor1_speed;         
int motor2_counter = 0;
int16_t motor2_speed;         
int motor3_counter = 0;
int16_t motor3_speed;

int f = 1;
boolean lock = 1;

int  tempL, tempH, tempOut;
float mpuTemp;

long currentT, previousT_1, previousT_2 = 0; 

void writeTo(byte device, byte address, byte value) {
  Wire.beginTransmission(device);
  Wire.write(address);
  Wire.write(value);
  Wire.endTransmission(true);
}

void beep() {
  digitalWrite(BUZZER, HIGH);
  delay(70);
  digitalWrite(BUZZER, LOW);
  delay(80);
}

void battVoltage(double voltage) {
  if (voltage > 8 && voltage <= 9.5) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }
  Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
}

void pwmSet(uint8_t channel, uint32_t value) {
  ledcWrite(channel, value);
}

void Motor1_control(int sp) {
   //Serial.print("PWM1: "); Serial.println(sp); //debug
  if (sp < 0) digitalWrite(DIR1, LOW);
  else digitalWrite(DIR1, HIGH);
  pwmSet(PWM1_CH, 255 - abs(sp));
}

void Motor2_control(int sp) {
   //Serial.print("PWM2: "); Serial.println(sp); //debug
  if (sp < 0) digitalWrite(DIR2, LOW);
  else digitalWrite(DIR2, HIGH);
  pwmSet(PWM2_CH, 255 - abs(sp));
}

void Motor3_control(int sp) {
   //Serial.print("PWM3: "); Serial.println(sp); //debug
  if (sp < 0) digitalWrite(DIR3, LOW);
  else digitalWrite(DIR3, HIGH);
  pwmSet(PWM3_CH, 255 - abs(sp));
}

void ENC1_READ() {
  static int state = 0;
  state = (state << 2 | (digitalRead(ENC1_1) << 1) | digitalRead(ENC1_2)) & 0x0f;
  if (state == 0x02 || state == 0x0d || state == 0x04 || state == 0x0b) {
    enc_count1++;
  } else if (state == 0x01 || state == 0x0e || state == 0x08 || state == 0x07) {
    enc_count1--;
  }
}

void ENC2_READ() {
  static int state = 0;
  state = (state << 2 | (digitalRead(ENC2_1) << 1) | digitalRead(ENC2_2)) & 0x0f;
  if (state == 0x02 || state == 0x0d || state == 0x04 || state == 0x0b) {
    enc_count2++;
  } else if (state == 0x01 || state == 0x0e || state == 0x08 || state == 0x07) {
    enc_count2--;
  }
}

void ENC3_READ() {
  static int state = 0;
  state = (state << 2 | (digitalRead(ENC3_1) << 1) | digitalRead(ENC3_2)) & 0x0f;
  if (state == 0x02 || state == 0x0d || state == 0x04 || state == 0x0b) {
    enc_count3++;
  } else if (state == 0x01 || state == 0x0e || state == 0x08 || state == 0x07) {
    enc_count3--;
  }
}

void setup() {
  Serial.begin(115200);
  
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
  
  Wire.begin();
  beep();
  Serial.println();
  Serial.println();
}

void loop() {

  currentT = millis();

  if (currentT - previousT_1 >= 30) {

    motor1_speed = enc_count1;
    enc_count1 = 0;
    motor2_speed = enc_count2;
    enc_count2 = 0;
    motor3_speed = enc_count3;
    enc_count3 = 0;

    switch (f) {
      case 1:
        if (lock) Serial.println("Motors testing... Be careful! Do not touch wheels!");
        lock = 0;
        break;
      case 2:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Slow rotating real wheel");
          Motor1_control(-30);
          lock = 0;
        }
        break;
      case 3:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Speed up");
          Motor1_control(-60);
          lock = 0;
        }
        break; 
      case 4:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Speed up");
          Motor1_control(-110);
          lock = 0;
        }
        break;    
      case 5:
        if (lock) {
          digitalWrite(BRAKE, LOW);
          Serial.println("Stop");
          Motor1_control(0);
          lock = 0;
        }
        break;      
      case 6:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Slow rotating real wheel (back)");
          Motor1_control(30);
          lock = 0;
        }
        break;
      case 7:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Speed up");
          Motor1_control(60);
          lock = 0;
        }
        break; 
      case 8:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Speed up");
          Motor1_control(110);
          lock = 0;
        }
        break;    
      case 9:
        if (lock) {
          digitalWrite(BRAKE, LOW);
          Serial.println("Stop");
          Motor1_control(0);
          lock = 0;
        }
        break; 
      case 10:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Checking encoder_1...");
          Motor1_control(-60);
          lock = 0;
        }
        break; 
     case 11:
        if (lock && motor1_speed < -80) {
          Serial.println("Encoder OK");
          Serial.print("Speed: "); Serial.println(motor1_speed);
          Serial.println("Stop");
        } else if (lock && motor1_speed >= 0) {
          Serial.println("Encoder FAIL!");
          Serial.print("Speed: "); Serial.println(motor1_speed);
          Serial.println("Stop");
        }
        digitalWrite(BRAKE, LOW);
        Motor1_control(0);
        lock = 0;
        break;       
     case 12:
        
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Slow rotating reaction wheel_1");
          Motor2_control(30);
          lock = 0;
        }
        break;
      case 13:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Speed up");
          Motor2_control(60);
          lock = 0;
        }
        break; 
      case 14:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Speed up");
          Motor2_control(110);
          lock = 0;
        }
        break;    
      case 15:
        if (lock) {
          digitalWrite(BRAKE, LOW);
          Serial.println("Stop");
          Motor2_control(0);
          lock = 0;
        }
        break;      
      case 16:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Slow rotating reaction wheel_1 (back)");
          Motor2_control(-30);
          lock = 0;
        }
        break;
      case 17:
        if (lock) { 
          digitalWrite(BRAKE, HIGH);
          Serial.println("Speed up");
          Motor2_control(-60);
          lock = 0;
        }
        break; 
      case 18:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Speed up");
          Motor2_control(-110);
          lock = 0;
        }
        break;    
      case 19:
        if (lock) {
          digitalWrite(BRAKE, LOW);
          Serial.println("Stop");
          Motor2_control(0);
          lock = 0;
        }
        break;
      case 20:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Checking encoder_2...");
          Motor2_control(-60);
          lock = 0;
        }
        break; 
      case 21:
        if (lock && motor2_speed < -80) {
          Serial.println("Encoder OK");
          Serial.print("Speed: "); Serial.println(motor2_speed);
          Serial.println("Stop");
        } else if (lock && motor2_speed >= 0) {
          Serial.println("Encoder FAIL!");
          Serial.print("Speed: "); Serial.println(motor2_speed);
          Serial.println("Stop");
        }
        digitalWrite(BRAKE, LOW);
        Motor2_control(0);
        lock = 0;
        break;
      case 22:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Slow rotating reaction wheel_2");
          Motor3_control(30);
          lock = 0;
        }
        break;
      case 23:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Speed up");
          Motor3_control(60);
          lock = 0;
        }
        break; 
      case 24:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Speed up");
          Motor3_control(110);
          lock = 0;
        }
        break;    
      case 25:
        if (lock) {
          digitalWrite(BRAKE, LOW);
          Serial.println("Stop");
          Motor3_control(0);
          lock = 0;
        }
        break;      
      case 26:
        if (lock) { 
          digitalWrite(BRAKE, HIGH);
          Serial.println("Slow rotating reaction wheel_2 (back)");
          Motor3_control(-30);
          lock = 0;
        }
        break;
      case 27:
        if (lock) {
          Serial.println("Speed up");
          digitalWrite(BRAKE, HIGH);
          Motor3_control(-60);
          lock = 0;
        }
        break; 
      case 28:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Speed up");
          Motor3_control(-110);
          lock = 0;
        }
        break;    
      case 29:
        if (lock) {
          digitalWrite(BRAKE, LOW);
          Serial.println("Stop");
          Motor3_control(0);
          lock = 0;
        }
        break;
      case 30:
        if (lock) {
          digitalWrite(BRAKE, HIGH);
          Serial.println("Checking encoder_2...");
          Motor3_control(-60);
          lock = 0;
        }
        break; 
      case 31:
        if (lock && motor3_speed < -80) {
          Serial.println("Encoder OK");
          Serial.print("Speed: "); Serial.println(motor3_speed);
          Serial.println("Stop");
        } else if (lock && motor3_speed >= 0) {
          Serial.println("Encoder FAIL!");
          Serial.print("Speed: "); Serial.println(motor3_speed);
          Serial.println("Stop");
        }
        digitalWrite(BRAKE, LOW);
        Motor3_control(0);
        lock = 0;
        break;  
      case 32:
        if (lock) {
          Serial.println("Trying to communicate with MPU6050...");
          Wire.beginTransmission(MPUaddr);                 // Start communication with the MPU-6050.
          Wire.write(0x6B);                                // We want to write to the PWR_MGMT_1 register (6B hex).
          Wire.write(0x00);                                // enable temp sensor         
          Wire.endTransmission();
        
          Wire.beginTransmission(MPUaddr);                 // Start communication with the MPU-6050.
          Wire.write(0x41);                                // pointing Temp_Out_High Reg     
          Wire.endTransmission();
        
          Wire.requestFrom(MPUaddr, 2);                   // two-byte temp data               
          int16_t temp = Wire.read() << 8 | Wire.read();
        
          float mpuTemp = (temp / 340.0 + 36.53);         // formula from data sheets
          Serial.print("Temp = ");
          Serial.print(mpuTemp, 2);                       // 2-digit after decimal point
          Serial.println(" degC");
          Serial.println("If you see temperature, MPU6050 OK");
          lock = 0; 
        }
        break;
      case 33:
        if (lock) {
          Serial.println("Trying to get battery voltage...");
          battVoltage((double)analogRead(VBAT) / 204);
          Serial.println("Are you sure you see the correct voltage?"); 
          lock = 0;
        }
        break;
    }
    previousT_1 = currentT;
  }
  
  if (currentT - previousT_2 >= 2500) { 
     f++;
     if (f == 35) {
      f = 1;
      Serial.println("Beep! Beep!"); Serial.println();
      beep(); beep(); 
     }
     lock = 1;
     previousT_2 = currentT;
  }
}


