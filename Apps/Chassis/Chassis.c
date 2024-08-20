/**
 * @file Chassis.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "chassis.h"
#include "StepMotor.h"
#include "usart.h"

// static StepMotorInstance *LeftForwardMotorInstance,
//                          *RightForwardMotorInstance,
//                          *RightBackMotorInstance,
//                          *LeftBackMotorInstance;

void ChassisInit()
{
    //填写设置参数
    StepMotor_Init_Config_s ChassisMotor_Init_Config = {
        .control_id = 0x01,
        .control_mode = ForceMode,
        .motor_direction = CounterClockWise,
        .subdivision = 0x20,
        .data = 600,
        .speed = 36,
        .control = &StepMotorControl,
    };

    //注册四个步进电机
    ChassisMotor_Init_Config.usart_handle = &huart1;
    StepMotorInstance* LeftForwardMotorInstance = StepMotorRegister(&ChassisMotor_Init_Config);
    ChassisMotor_Init_Config.usart_handle = &huart2;
    StepMotorInstance* RightForwardMotorInstance = StepMotorRegister(&ChassisMotor_Init_Config);
    ChassisMotor_Init_Config.usart_handle = &huart3;
    StepMotorInstance* RightBackMotorInstance = StepMotorRegister(&ChassisMotor_Init_Config);
    ChassisMotor_Init_Config.usart_handle = &huart4;
    StepMotorInstance* LeftBackMotorInstance = StepMotorRegister(&ChassisMotor_Init_Config);
    //控制四个步进电机
    LeftForwardMotorInstance->control(LeftForwardMotorInstance);
    RightForwardMotorInstance->control(RightForwardMotorInstance);
    RightBackMotorInstance->control(RightBackMotorInstance);
    LeftBackMotorInstance->control(LeftBackMotorInstance);

}