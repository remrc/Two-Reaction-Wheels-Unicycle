#define BUZZER      27
#define VBAT        34
#define INT_LED     2

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

#define MPU6050 0x68              // Device address
#define ACCEL_CONFIG 0x1C         // Accelerometer configuration address
#define GYRO_CONFIG  0x1B         // Gyro configuration address

#define EEPROM_SIZE 96

#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C

#define accSens 0             // 0 = 2g, 1 = 4g, 2 = 8g, 3 = 16g
#define gyroSens 1            // 0 = 250rad/s, 1 = 500rad/s, 2 1000rad/s, 3 = 2000rad/s

#define Gyro_amount 0.996

#define STEERING_MAX    15
#define SPEED_MAX       40

int steering_remote = 0, speed_remote = 0;
int rotation;

bool vertical = false;
bool calibrating = false;
bool calibrated = false;
bool go_straight = false;
bool rot_correction = false;
int batt_divider = 204; // divider must be selected to measure the battery voltage correctly

float loop_time = 15;

float K1GainY = 24;
float K2GainY = 2.7;
float K3GainY = 2.5;
float K4GainY = 0.13;
float alphaY = 0.80;

float K1GainX = 185;
float K2GainX = 7.0;
float K3GainX = 0.38;
float K4GainX = 0.11;
float alphaX = 0.70;

struct AccOffsetsObj {
  int ID;
  int16_t X;
  int16_t Y;
  int16_t Z;
};
AccOffsetsObj offsets;

int16_t  AcX, AcY, AcZ, AcXc, AcYc, AcZc, GyY, gyroY, GyX, gyroX, GyZ, gyroZ, gyroXfilt, gyroYfilt;

int16_t  GyX_offset = 0;
int16_t  GyY_offset = 0;
int16_t  GyZ_offset = 0;
int32_t  GyX_offset_sum = 0;
int32_t  GyY_offset_sum = 0;
int32_t  GyZ_offset_sum = 0;

float robot_angleX, robot_angleY, angleX, angleY, rotation_angle;
float Acc_angleX, Acc_angleY;      

long currentT, previousT_1, previousT_2;

volatile int  enc_count1 = 0, enc_count2 = 0, enc_count3 = 0;
int16_t motorY_speed, motorX_speed;
int32_t motorY_pos, motorX_pos;

int motor1_counter = 0;
int16_t motor1_speed;         
int motor2_counter = 0;
int16_t motor2_speed;         
int motor3_counter = 0;
int16_t motor3_speed;         
