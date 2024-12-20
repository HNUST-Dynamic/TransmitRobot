// /**
//  * @file Lift.c
//  * @author Libaoen18
//  * @brief  电梯平台程序（包括物料盘转动）
//  * @version 0.1
//  * @date 2024-08-27
//  *
//  * @copyright Copyright (c) 2024
//  *
//  */
#include "Lift.h"
#include "ServoMotor.h"
#include "StepMotor.h"
#include <math.h>
#include "Cmd.h"

#define WHEEL_RADIUS 0.01f // 轮子半径（米）
#define PI 3.1415926f
// UART_HandleTypeDef  huart6;
TIM_OC_InitTypeDef sConfigOC;

ServoInstance *GripperServoMotor_Instance, // 抓手舵机        通道三
    *ElevatorServoMotor_Instance,          // 电梯下盘舵机     通道二
    *TurntableServoMotor_Instance;         // 物料盘舵机       通道一

static StepMotorInstance *ElevatorMotorInstance;

// 注册三个舵机
void Lift_Init()
{
  // ServoMotor_Init();
  Servo_Init_Config_s LiftServoMotor_Config = {
      .angle = 0,
      .htim = &htim1,
      .Channel = TIM_CHANNEL_2,
  };
  // 注册三个舵机
  LiftServoMotor_Config.angle = 0;
  LiftServoMotor_Config.Channel = TIM_CHANNEL_2;
  GripperServoMotor_Instance = ServoInit(&LiftServoMotor_Config);

  LiftServoMotor_Config.angle = 0;
  LiftServoMotor_Config.Channel = TIM_CHANNEL_3;
  ElevatorServoMotor_Instance = ServoInit(&LiftServoMotor_Config);

  LiftServoMotor_Config.angle = 0;
  LiftServoMotor_Config.Channel = TIM_CHANNEL_4;
  TurntableServoMotor_Instance = ServoInit(&LiftServoMotor_Config);

  HAL_TIM_PWM_Init(&htim1);
  // HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2);
  // HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3);
  // HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4); /* 配置TIMx通道y */
}

// 抓手抓取
void pickup()
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4); /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 500);
}

// 抓手松开
void putdown()
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4); /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 880);
}

// 电梯由内转向外
void Lift_Turn()
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); /* 开启对应PWM通道 */
  // HAL_Delay(20);
  //__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,1130);
  // HAL_Delay(20);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 2110);
}

// 电梯由外转向内
void Lift_Turn_back()
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); /* 开启对应PWM通道 */
  HAL_Delay(20);
  //__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,1130);
  //  HAL_Delay(20);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 750);
}

// 物料盘把红色格子转到取放区
void Turn_Red()
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);           /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 1379); // 1389
}

// 物料盘把蓝色格子转到取放区
void Turn_Bule()
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);           /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 2268); // 2278
}

// 物料盘把绿色格子转到取放区
void Turn_Green()
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 500);
}

// 将视觉识别到的数据传入，物料盘就可以移动到对应的待取区
void Turn_Color(uint8_t element)
{
  switch (element)
  {
    case 0x33:
      Turn_Red();
      break;
    case 0x34:
      Turn_Green();
      break;
    case 0x35:
      Turn_Bule();
      break;
    default:
      break;
  }
}

// 将视觉识别到的数据传入，物料盘就可以移动到对应的待取区
void Turn_Color_two(uint8_t element)
{
  switch (element)
  {
    case 0x31:
      Turn_Red();
      break;
    case 0x32:
      Turn_Green();
      break;
    case 0x33:
      Turn_Bule();
      break;
    default:
      break;
  }
}

// 电梯的方向：down向下；up向上；
void ElevatorMotor_Init()
{
  StepMotor_Init_Config_s ElevatorMotor_Init_Config = {
      .step_mode = PosMode,
      .ctrl_mode = CloseCircuit,
      .motor_direction = CounterClockWise,
      .acc = 0x00,
      .speed = 150,
  };

  ElevatorMotor_Init_Config.usart_handle = &huart5;
  ElevatorMotorInstance = StepMotorRegister(&ElevatorMotor_Init_Config);

  StepMotorResetZero(ElevatorMotorInstance);
  HAL_Delay(200);
  StepMotorModifyCtrlMode(ElevatorMotorInstance, true);
  HAL_Delay(200);
  StepMotorEnControl(ElevatorMotorInstance, true, false);
  HAL_Delay(200);
}

// 控制电梯上下的程序，参数需要修改了，参数为方向、速度、距离
void Lift_updown_control(Lift_Direction_e Direction, uint16_t Velocity, uint32_t Length)
{
  ElevatorMotorInstance->step_mode = PosMode;

  ElevatorMotorInstance->motor_direction = Direction;

  ElevatorMotorInstance->speed = Velocity;
  ElevatorMotorInstance->clk = Length;

  // StepMotorEnControl(ElevatorMotorInstance, true, false);
  // HAL_Delay(200);
  StepMotorPosControl(ElevatorMotorInstance, false, false);
}

// 初始展开
void Lift_StartFirst()
{
  // Turn把电梯转出去参数为：
  Lift_Turn();
  putdown();
  HAL_Delay(800);
  // Down把电梯放下来
}

// 抓取第一区的物料然后放在物料盘，一遍
void Lift_Catch(uint8_t element)
{
  Turn_Color(element);
  Lift_updown_control(down, 2000, 46000);
  HAL_Delay(500);
  pickup();
  HAL_Delay(200);
  Lift_updown_control(up, 2000, 46000);
  // HAL_Delay(800);

  HAL_Delay(500);
  Lift_Turn_back();
  HAL_Delay(800);
  Lift_updown_control(down, 2000, 35000);
  HAL_Delay(500);
  putdown();
  HAL_Delay(200);
  Lift_updown_control(up, 2000, 35000);
  HAL_Delay(500);
}

void Lift_Catch_two(uint8_t element) // 把物料从地面上抓到物料盘里
{
  Turn_Color(element);
  Lift_updown_control(down, 2000, 35000);
  pickup();
  HAL_Delay(200);
  // Lift_updown_control(up,1000,170000);
  HAL_Delay(500);
  Lift_Turn_back();
  HAL_Delay(800);
  putdown();
  HAL_Delay(400);
  Lift_updown_control(up, 2000, 35000);
  HAL_Delay(1000);
}

// 把物料放在物料盘里后，把电梯转外面
void Lift_Back()
{
  Lift_Turn();
  // Lift_updown_control(up,1000,210000);
  putdown();
}

// 在粗加工、存储区把物料盘上的物料放下的操作，检查了逻辑一遍
void Goods_Putdown(uint8_t element)
{
  Turn_Color_two(element);
  // HAL_Delay(1000);
  Lift_Turn_back();
  HAL_Delay(800);
  Lift_updown_control(down, 2400, 36000);
  HAL_Delay(1000);
  pickup();
  HAL_Delay(300);
  Lift_updown_control(up, 2500, 36000);
  HAL_Delay(1000);
  Lift_Turn();
  Lift_updown_control(down, 600, 98000);
  HAL_Delay(2300);
  putdown();
  HAL_Delay(200);
  Lift_updown_control(up, 2500, 98000);
  HAL_Delay(1800);
}

// 在粗加工、存储区把物料盘上的物料拿上的操作
void Goods_Pickup(uint8_t element)
{
  Turn_Color_two(element);
  // HAL_Delay(1000);
  Lift_updown_control(down, 2500, 98000);
  HAL_Delay(2300);
  pickup();
  HAL_Delay(200);
  Lift_updown_control(up, 2500, 98000);
  HAL_Delay(1800);

  Turn_Color_two(element);
  // HAL_Delay(1000);
  Lift_Turn_back();
  HAL_Delay(800);
  Lift_updown_control(down, 2500, 35000);
  HAL_Delay(800);
  putdown();
  HAL_Delay(200);
  Lift_updown_control(up, 2500, 35000);
  HAL_Delay(800);
}
// 码垛
void Modes_Putdown(uint8_t element)
{
  Turn_Color_two(element);
  // HAL_Delay(1000);
  Lift_Turn_back();
  HAL_Delay(800);
  Lift_updown_control(down, 2500, 36000);
  HAL_Delay(1000);
  pickup();
  HAL_Delay(500);
  Lift_updown_control(up, 2500, 36000);
  HAL_Delay(1000);
  Lift_Turn();
  Lift_updown_control(down, 600, 32000);
  HAL_Delay(1800);
  putdown();
  HAL_Delay(200);
  Lift_updown_control(up, 2500, 32000);
  HAL_Delay(1800);
}

