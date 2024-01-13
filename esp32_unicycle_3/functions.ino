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

void angle_setup() {
  Wire.begin();
  delay (100);
  writeTo(MPU6050, PWR_MGMT_1, 0);
  writeTo(MPU6050, ACCEL_CONFIG, accSens << 3); // Specifying output scaling of accelerometer
  writeTo(MPU6050, GYRO_CONFIG, gyroSens << 3); // Specifying output scaling of gyroscope
  delay (100);

  for (int i = 0; i < 512; i++) {
    angle_calc();
    GyX_offset_sum += GyX;
    delay(3);
  }
  GyX_offset = GyX_offset_sum >> 9;
  Serial.print("GyX offset value = "); Serial.println(GyX_offset);
  beep();
  for (int i = 0; i < 512; i++) {
    angle_calc();
    GyY_offset_sum += GyY;
    delay(3);
  }
  GyY_offset = GyY_offset_sum >> 9;
  Serial.print("GyY offset value = "); Serial.println(GyY_offset);
  beep();
  for (int i = 0; i < 512; i++) {
    angle_calc();
    GyZ_offset_sum += GyZ;
    delay(3);
  }
  GyZ_offset = GyZ_offset_sum >> 9;
  Serial.print("GyZ offset value = "); Serial.println(GyZ_offset);

  beep();
  beep();
}

void angle_calc() {

  Wire.beginTransmission(MPU6050);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050, 6, true);  
  AcX = Wire.read() << 8 | Wire.read(); 
  AcY = Wire.read() << 8 | Wire.read(); 
  AcZ = Wire.read() << 8 | Wire.read(); 
  
  Wire.beginTransmission(MPU6050);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050, 6, true);  
  GyX = Wire.read() << 8 | Wire.read(); 
  GyY = Wire.read() << 8 | Wire.read(); 
  GyZ = Wire.read() << 8 | Wire.read(); 

  AcXc = AcX - offsets.X;
  AcYc = AcY - offsets.Y;
  AcZc = AcZ - offsets.Z;
  GyX -= GyX_offset;
  GyY -= GyY_offset;
  GyZ -= GyZ_offset;

  robot_angleY += GyY * loop_time / 1000 / 65.536;
  Acc_angleY = -atan2(AcXc, AcZc) * 57.2958;
  robot_angleY = robot_angleY * Gyro_amount + Acc_angleY * (1.0 - Gyro_amount);
               
  robot_angleX += GyX * loop_time / 1000 / 65.536;
  Acc_angleX = atan2(AcYc, AcZc) * 57.2958;
  robot_angleX = robot_angleX * Gyro_amount + Acc_angleX * (1.0 - Gyro_amount);

  if (abs(robot_angleY) < 0.7 && abs(robot_angleX) < 0.5) {
    vertical = true;
  } else if (abs(robot_angleY) > 10 || abs(robot_angleX) > 6) {
    vertical = false;
  }
}

void battVoltage(double voltage) {
  //SerialBT.print("batt: "); SerialBT.println(voltage); // debug
  if (voltage > 8 && voltage <= 9.5) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }
}

void pwmSet(uint8_t channel, uint32_t value) {
  ledcWrite(channel, value);
}

void Motor1_control(int sp) {
  if (sp < 0) digitalWrite(DIR1, LOW);
  else digitalWrite(DIR1, HIGH);
  pwmSet(PWM1_CH, 255 - abs(sp));
}

void Motor2_control(int sp) {
  if (sp < 0) digitalWrite(DIR2, LOW);
  else digitalWrite(DIR2, HIGH);
  pwmSet(PWM2_CH, 255 - abs(sp));
}

void Motor3_control(int sp) {
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

int Tuning() {
  if (!SerialBT.available())  return 0;
  char param = SerialBT.read();               // get parameter byte
  if (!SerialBT.available()) return 0;
  char cmd = SerialBT.read();                 // get command byte
  switch (param) {
    case 'b':
      if (cmd == '+')    K1GainX += 2;
      if (cmd == '-')    K1GainX -= 2;
      printX_values();
      break;
    case 'y':
      if (cmd == '+')    K2GainX += 0.5;
      if (cmd == '-')    K2GainX -= 0.5;
      printX_values();
      break;  
    case 'z':
      if (cmd == '+')    K3GainX += 0.01;
      if (cmd == '-')    K3GainX -= 0.01;
      printX_values();
      break;  
     case 'e':
      if (cmd == '+')    K4GainX += 0.01;
      if (cmd == '-')    K4GainX -= 0.01;
      printX_values();
      break;    
    case 'p':
      if (cmd == '+')    K1GainY += 1;
      if (cmd == '-')    K1GainY -= 1;
      printY_values();
      break;
    case 'i':
      if (cmd == '+')    K2GainY += 0.1;
      if (cmd == '-')    K2GainY -= 0.1;
      printY_values();
      break;  
    case 's':
      if (cmd == '+')    K3GainY += 0.1;
      if (cmd == '-')    K3GainY -= 0.1;
      printY_values();
      break;  
    case 'a':
      if (cmd == '+')    K4GainY += 0.005;
      if (cmd == '-')    K4GainY -= 0.005;
      printY_values();
      break;    
    case 'm':
      if (cmd == '+' && rotation < 40)    rotation += 20;
      if (cmd == '-' && rotation > -40)   rotation -= 20;
      printR_values();
      break;          
    case 'c':
      if (cmd == '+' && !calibrating) {
        calibrating = true;
        SerialBT.println("Calibrating on");
        SerialBT.println("hold robot still on balancing point");
      }
      if (cmd == '-' && calibrating) { 
        SerialBT.print("X: "); SerialBT.print(AcX); SerialBT.print(" Y: "); SerialBT.print(AcY); SerialBT.print(" Z: "); SerialBT.println(AcZ - 16384);
        if (abs(AcX) < 3000 && abs(AcY) < 3000) {
          offsets.ID = 29;
          offsets.X = AcX;
          offsets.Y = AcY;
          offsets.Z = AcZ - 16384;
          beep();
          SerialBT.println("calibrating complete.");
          EEPROM.put(0, offsets);
          EEPROM.commit();
          calibrating = false;
          calibrated = true;
        } else {
          calibrating = false;
          SerialBT.println("The angles are wrong!!!");
          SerialBT.println("calibrating off...");
          beep();
          beep();
        }
      }                           
   }
   return 1;  
}

void printX_values() {
  SerialBT.print("K1X(b): "); SerialBT.print(K1GainX);
  SerialBT.print(" K2X(y): "); SerialBT.print(K2GainX);
  SerialBT.print(" K3X(z): "); SerialBT.print(K3GainX,4);
  SerialBT.print(" K4X(e): "); SerialBT.println(K4GainX,4);
}

void printY_values() {
  SerialBT.print("K1Y(p): "); SerialBT.print(K1GainY);
  SerialBT.print(" K2Y(i): "); SerialBT.print(K2GainY);
  SerialBT.print(" K3Y(s): "); SerialBT.print(K3GainY,4);
  SerialBT.print(" K4Y(a): "); SerialBT.println(K4GainY,4);
}

void printR_values() {
  SerialBT.print("Rotation(m): "); SerialBT.println(rotation);
}
