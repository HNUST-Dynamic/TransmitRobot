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


#include "Chassis.h"
#include "StepMotor.h"
#include "usart.h"
#include <math.h>

static ServoInstance     *GripperServoMotor_Instance,    // 抓手舵机
                         *ElevatorServoMotor_Instance,   //电梯下盘舵机
                         *TurntableServoMotor_Instance;  //物料盘舵机

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

}

void pickup()//抓手抓取，参数要调整
{
    ServoMotor_Set_Angle(GripperServoMotor_Instance,10);
}
void putdown()
{
    ServoMotor_Set_Angle(GripperServoMotor_Instance,270);
}

void Lift_Turn()//电梯转正向
{
    ServoMotor_Set_Angle(ElevatorServoMotor_Instance,180);
}

void Lift_Turn_back()//电梯转反向
{
    ServoMotor_Set_Angle(ElevatorServoMotor_Instance,0);
}

void TurnTabble_Turn()//物料盘转一格，这个肯定是要改的，因为放和取物料的顺序不一样
{
    ServoMotor_Set_Angle(TurntableServoMotor_Instance,180);
}



void ElevatorMotor_Init()
{
    StepMotor_Init_Config_s ElevatorMotor_Init_Config = {
     .control_id = 0x01,
     .control_mode = ForceMode,
     .motor_direction = CounterClockWise,
     .subdivision = 0x20,
     .data = 1200,
     .speed = 50,
     .control = &StepMotorControl,
    };

    ElevatorMotor_Init_Config.usart_handle = &huart1;
    ElevatorMotorInstance = StepMotorRegister(&ElevatorMotor_Init_Config);

    ElevatorMotorInstance->control(ElevatorMotorInstance);

}

void Lift_updown_control(Chassis_Direction_e Direction,float Velocity,float Length)
{
   ElevatorMotorInstance->control_mode = PosMode;

   ElevatorMotorInstance->motor_direction = CounterClockWise;

   ElevatorMotorInstance->control(ElevatorMotorInstance);

}