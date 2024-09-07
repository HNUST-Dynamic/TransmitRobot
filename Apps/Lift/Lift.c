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

 ServoInstance     *GripperServoMotor_Instance,    // 抓手舵机
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
void angle_tset()
{
    ServoMotor_Set_Angle(GripperServoMotor_Instance,270);
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

void Lift_updown_control(Chassis_Direction_e Direction,uint16_t Velocity,uint32_t Length)
{
   ElevatorMotorInstance->step_mode = PosMode;

   ElevatorMotorInstance->motor_direction = CounterClockWise;

   ElevatorMotorInstance->speed = Velocity;
   ElevatorMotorInstance->clk =  Length;
   
   StepMotorEnControl(ElevatorMotorInstance,true,false);
   HAL_Delay(200);
   StepMotorPosControl(ElevatorMotorInstance,false,false);
}

//启动之后将电梯升起然后转向出去
void Lift_StartFirst()
{
    //Top
    //ElevatorMotor_Init();
    //HAL_Delay(1000);
    Lift_updown_control(Back,1000,30000);
    //Turn








}

void Lift_wholeProcess()
{







}