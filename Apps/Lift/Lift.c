/**
 * @file Lift.c
 * @author Libaoen18
 * @brief  电梯平台程序（包括物料盘转动）
 * @version 0.1
 * @date 2024-08-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "Lift.h"
#include "ServoMotor.h"
#include "StepMotor.h"
#include <math.h>


#define WHEEL_RADIUS 0.01f // 轮子半径（米）
#define PI           3.1415926f


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
    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2);
    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3);
    HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4); /* 配置TIMx通道y */

}

void pickup()//抓手抓取，参数要调整
{
   HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);                     /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4, 1500);
}
void putdown()
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);                     /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4, 2000);
}

void Lift_Turn()//电梯转外向
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);                     /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3, 2500);
}

void Lift_Turn_back()//电梯转内向
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);                     /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3, 1150);
}

void TurnTabble_Turn()//物料盘转一格，这个肯定是要改的，因为放和取物料的顺序不一样
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);                     /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2, 2500);
}
void Turn_Red()
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);                     /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2, 200);

}

void Turn_Bule()
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);                     /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2, 2000);

}
void Turn_Green()
{
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);                     /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2, 1110);

}

void angle_tset()
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);                     /* 开启对应PWM通道 */
  __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2, 2500);
}

// 电梯的方向：Forward-向下；Back-向上；
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

//启动之后将电梯升起然后转向出去
void Lift_StartFirst()
{
    //Top升到顶
    Lift_updown_control(up,1000,210000);
    //Turn把电梯转出去参数为：
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);                     /* 开启对应PWM通道 */
    __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3, 500);//3771
    //Down把电梯放下来
    Lift_updown_control(down,1000,210000);

}

void LiftwholeProcess()
{











    
}