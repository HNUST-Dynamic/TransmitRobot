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
#define PI           3.1415926f
extern int X;
//UART_HandleTypeDef  huart6;
TIM_OC_InitTypeDef sConfigOC;

 ServoInstance     *GripperServoMotor_Instance,     // 抓手舵机        通道三
                   *ElevatorServoMotor_Instance,    //电梯下盘舵机     通道二
                   *TurntableServoMotor_Instance;   //物料盘舵机       通道一


static StepMotorInstance *ElevatorMotorInstance;

//注册三个舵机
void Lift_Init()
{
    //ServoMotor_Init();
    Servo_Init_Config_s LiftServoMotor_Config = {
        .angle   = 0 ,
        .htim    = &htim1,
        .Channel = TIM_CHANNEL_2,
    };
    // 注册三个舵机
    LiftServoMotor_Config.angle    = 0;
    LiftServoMotor_Config.Channel  = TIM_CHANNEL_2 ;
    GripperServoMotor_Instance     = ServoInit(&LiftServoMotor_Config);

    LiftServoMotor_Config.angle    = 0;
    LiftServoMotor_Config.Channel  = TIM_CHANNEL_3 ;
    ElevatorServoMotor_Instance    = ServoInit(&LiftServoMotor_Config);

    LiftServoMotor_Config.angle    = 0;
    LiftServoMotor_Config.Channel  = TIM_CHANNEL_4 ;
    TurntableServoMotor_Instance   = ServoInit(&LiftServoMotor_Config);

    HAL_TIM_PWM_Init(&htim1);
    // HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2);
    // HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3);
    // HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4); /* 配置TIMx通道y */

}

void pickup()//抓手抓取
{
   HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);                     /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4, 1000);
}
void putdown()//抓手松开
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);                     /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,710);
}

void Lift_Turn()//电梯由
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);                     /* 开启对应PWM通道 */
  //HAL_Delay(20);
  //__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,1130);
  //HAL_Delay(20);
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,750);

}

void Lift_Turn_back()//电梯由
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);                     /* 开启对应PWM通道 */
  HAL_Delay(20);
  //__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,1130);
  //  HAL_Delay(20);
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,2150);
}
void TurnTabble_Turn()//这个不用了
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);                     /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,2500);
}
void Turn_Red()//物料盘把红色格子转到取放区
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);                     /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,1389);

}

void Turn_Bule()//物料盘把蓝色格子转到取放区
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);                     /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,2278);

}
void Turn_Green()//物料盘把绿色格子转到取放区
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);                     /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,500);

}

void angle_tset()//不需要了
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);                     /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,20000-2500);
}

void Turn_Color(uint8_t element)//将视觉识别到的数据传入，物料盘就可以移动到对应的待取区
{
  switch (element)
  {
  case 0x33 : 
  Turn_Red();
    break;
  case 0x34 : 
  Turn_Green();
    break;
  case 0x35 : 
  Turn_Bule();
    break;
  default:
    break;
  }
}
void Turn_Color_two(uint8_t element)//将视觉识别到的数据传入，物料盘就可以移动到对应的待取区
{
  switch (element)
  {
  case 0x31 : 
  Turn_Red();
    break;
  case 0x32 : 
  Turn_Green();
    break;
  case 0x33 : 
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
    .acc = 0,
    .speed = 50,
    };


    ElevatorMotor_Init_Config.usart_handle = &huart5;
    ElevatorMotorInstance = StepMotorRegister(&ElevatorMotor_Init_Config);

    StepMotorResetZero(ElevatorMotorInstance);
    HAL_Delay(200);
    StepMotorModifyCtrlMode(ElevatorMotorInstance,true);
    HAL_Delay(200);
    StepMotorEnControl(ElevatorMotorInstance,true,false);
    HAL_Delay(200);

}
//控制电梯上下的程序，参数需要修改了，参数为方向、速度、距离
void Lift_updown_control(Lift_Direction_e Direction,uint16_t Velocity,uint32_t Length)
 {
    ElevatorMotorInstance->step_mode = PosMode;

    ElevatorMotorInstance->motor_direction = Direction;

    ElevatorMotorInstance->speed = Velocity;
    ElevatorMotorInstance->clk =  Length;
   
    StepMotorEnControl(ElevatorMotorInstance,true,false);
    HAL_Delay(200);
    StepMotorPosControl(ElevatorMotorInstance,false,false);
 }

 //初始展开
 void Lift_StartFirst()
 {
     //Turn把电梯转出去参数为：
     Lift_Turn();
     putdown();
     HAL_Delay(1000);
     //Down把电梯放下来

 }
 //抓取第一区的物料然后放在物料盘，一遍
 void Lift_Catch(uint8_t element)
 {
    Lift_updown_control(down,1000,60000);
    HAL_Delay(2000);

    pickup();
    HAL_Delay(1000);

    Lift_updown_control(up,1000,60000);
    HAL_Delay(2000);

    Turn_Color(element);
    HAL_Delay(1000);
    Lift_Turn_back();
    HAL_Delay(4000);
    Lift_updown_control(down,1000,40000);
    HAL_Delay(2000);    
    putdown();
    HAL_Delay(1000);
    Lift_updown_control(up,1000,40000);
    HAL_Delay(2000);

 }
void Lift_Catch_two(uint8_t element)//把物料从地面上抓到物料盘里
 {
    Lift_updown_control(down,1000,50000);
    pickup();
    HAL_Delay(20);
    //Lift_updown_control(up,1000,170000);
    HAL_Delay(1000);
    Turn_Color(element);
    HAL_Delay(1000);
    Lift_Turn_back();
    HAL_Delay(4000);
    
    HAL_Delay(2000);    
    putdown();
    HAL_Delay(1000);
    Lift_updown_control(up,1000,50000);
    HAL_Delay(2000);

 }
 //把物料放在物料盘里后，把电梯转外面
 void Lift_Back()
 {
    Lift_Turn();
    //Lift_updown_control(up,1000,210000);
    putdown();
 }
 //在粗加工、存储区把物料盘上的物料放下的操作，检查了逻辑一遍
void Goods_Putdown(uint8_t element)
 {
    Turn_Color_two(element);
    HAL_Delay(1000);
    Lift_Turn_back();
    HAL_Delay(1000);
    Lift_updown_control(down,1000,60000);
    HAL_Delay(3500);
    pickup();
    HAL_Delay(1000);
    Lift_updown_control(up,1000,60000);
    HAL_Delay(3500);
    Lift_Turn();
    Lift_updown_control(down,1000,200000);
    HAL_Delay(8000);
    putdown();
    HAL_Delay(1000);
    Lift_updown_control(up,1000,200000);
    HAL_Delay(8000);

 }
