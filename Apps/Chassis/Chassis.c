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
#include <math.h>

//实例化底盘四个电机
static StepMotorInstance *LeftForwardMotorInstance,
                        *RightForwardMotorInstance,
                        *RightBackMotorInstance,
                        *LeftBackMotorInstance;

void ChassisInit()
{
    //填写设置参数
    StepMotor_Init_Config_s ChassisMotor_Init_Config = {
        .control_id = 0x01,
        .control_mode = ForceMode,
        .motor_direction = CounterClockWise,
        .subdivision = 0x20,
        .data = 1200,
        .speed = 50,
    };

    //注册四个步进电机
    ChassisMotor_Init_Config.usart_handle = &huart1;
    LeftForwardMotorInstance = StepMotorRegister(&ChassisMotor_Init_Config);
    ChassisMotor_Init_Config.usart_handle = &huart2;
    RightForwardMotorInstance = StepMotorRegister(&ChassisMotor_Init_Config);
    ChassisMotor_Init_Config.usart_handle = &huart3;
    RightBackMotorInstance = StepMotorRegister(&ChassisMotor_Init_Config);
    ChassisMotor_Init_Config.usart_handle = &huart4;
    LeftBackMotorInstance = StepMotorRegister(&ChassisMotor_Init_Config);


}

/**
 * @brief           运动学正解算函数
 * 
 * @param vx        x方向线速度 （米/秒）
 * @param vy        y方向线速度 （米/秒）
 * @param omega     角速度（弧度/秒）逆时针为正
 */
void MecanumKinematics(float vx, float vy, float omega) 
{
    //vx+-vy 不能超过1.875
    // 计算每个轮子的转速
    LeftForwardMotorInstance->speed = (uint16_t)(fabs((vx + vy - (ROBOT_RADIUS * omega)) / WHEEL_RADIUS));  // 左前轮 单位rad/s
    RightForwardMotorInstance->speed = (uint16_t)(fabs((-vx + vy + (ROBOT_RADIUS * omega)) / WHEEL_RADIUS));  // 右前轮
    RightBackMotorInstance->speed = (uint16_t)(fabs((vx + vy + (ROBOT_RADIUS * omega)) / WHEEL_RADIUS));  // 右后轮
    LeftBackMotorInstance->speed = (uint16_t)(fabs((-vx + vy - (ROBOT_RADIUS * omega)) / WHEEL_RADIUS));  // 左后轮

}
/**
 * @brief           运动学逆解算函数
 * 
 * @param distance_x x方向移动的目标距离（米）
 * @param distance_y y方向移动的目标距离（米）
 */
void MecanumInverseKinematics(float distance_x,float distance_y)
{
    //distance_x +- distance_y 不能超过4.28
    LeftForwardMotorInstance->data = (uint16_t)(fabs(((distance_x + distance_y)*180) / (PI*WHEEL_RADIUS)));  // 左前轮 单位rad/s
    RightForwardMotorInstance->data = (uint16_t)(fabs(((-distance_x + distance_y)*180) / (PI*WHEEL_RADIUS)));  // 右前轮
    RightBackMotorInstance->data = (uint16_t)(fabs(((distance_x + distance_y)*180) / (PI*WHEEL_RADIUS)));  // 右后轮
    LeftBackMotorInstance->data = (uint16_t)(fabs(((-distance_x + distance_y)*180) / (PI*WHEEL_RADIUS)));  // 左后轮

}

void ChassisTransiation(Chassis_Direction_e Direction,float Velocity,float Length)
{
    //设置四个电机为pos模式
    LeftForwardMotorInstance->control_mode = PosMode;
    RightForwardMotorInstance->control_mode = PosMode;
    RightBackMotorInstance->control_mode = PosMode;
    LeftBackMotorInstance->control_mode = PosMode;

    switch(Direction)
    {
        case Forward:
            //设置四个电机的方向
            LeftForwardMotorInstance->motor_direction = CounterClockWise;
            RightForwardMotorInstance->motor_direction = CounterClockWise;
            RightBackMotorInstance->motor_direction = CounterClockWise;
            LeftBackMotorInstance->motor_direction = CounterClockWise;
            //设置四个电机的速度
            MecanumKinematics(0,Velocity,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(0,Length);
            break;
        case Back:
            //设置四个电机的方向
            LeftForwardMotorInstance->motor_direction = ClockWise;
            RightForwardMotorInstance->motor_direction = ClockWise;
            RightBackMotorInstance->motor_direction = ClockWise;
            LeftBackMotorInstance->motor_direction = ClockWise;
            //设置四个电机的速度
            MecanumKinematics(Velocity,0,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(Length,0);
            break;
        case Left:
            //设置四个电机的方向
            LeftForwardMotorInstance->motor_direction = CounterClockWise;
            RightForwardMotorInstance->motor_direction = ClockWise;
            RightBackMotorInstance->motor_direction = CounterClockWise;
            LeftBackMotorInstance->motor_direction = ClockWise;
            //设置四个电机的速度
            MecanumKinematics(Velocity,0,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(Length,0);
            break;
        case Right:
            //设置四个电机的方向
            LeftForwardMotorInstance->motor_direction = ClockWise;
            RightForwardMotorInstance->motor_direction = CounterClockWise;
            RightBackMotorInstance->motor_direction = ClockWise;
            LeftBackMotorInstance->motor_direction = CounterClockWise;
            //设置四个电机的速度
            MecanumKinematics(-Velocity,0,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(Length,0);
            break;
        case LeftForward:
            //设置四个电机的方向
            LeftForwardMotorInstance->motor_direction = CounterClockWise;
            RightForwardMotorInstance->motor_direction = CounterClockWise;
            RightBackMotorInstance->motor_direction = CounterClockWise;
            LeftBackMotorInstance->motor_direction = CounterClockWise;
            //设置四个电机的速度
            MecanumKinematics(0.707*Velocity,0.707*Velocity,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(0.707*Length,0.707*Length);
            break;
        case RightForward:
            //设置四个电机的方向
            LeftForwardMotorInstance->motor_direction = CounterClockWise;
            RightForwardMotorInstance->motor_direction = CounterClockWise;
            RightBackMotorInstance->motor_direction = CounterClockWise;
            LeftBackMotorInstance->motor_direction = CounterClockWise;
            //设置四个电机的速度
            MecanumKinematics(-0.707*Velocity,0.707*Velocity,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(0.707*Length,0.707*Length);
            break;
        case LeftBack:
            //设置四个电机的方向
            LeftForwardMotorInstance->motor_direction = ClockWise;
            RightForwardMotorInstance->motor_direction = ClockWise;
            RightBackMotorInstance->motor_direction = ClockWise;
            LeftBackMotorInstance->motor_direction = ClockWise;
            //设置四个电机的速度
            MecanumKinematics(0.707*Velocity,-0.707*Velocity,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(0.707*Length,0.707*Length);
            break;
        case RightBack:
            //设置四个电机的方向
            LeftForwardMotorInstance->motor_direction = ClockWise;
            RightForwardMotorInstance->motor_direction = ClockWise;
            RightBackMotorInstance->motor_direction = ClockWise;
            LeftBackMotorInstance->motor_direction = ClockWise;
            //设置四个电机的速度
            MecanumKinematics(-0.707*Velocity,-0.707*Velocity,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(0.707*Length,0.707*Length);
            break;
        default:
            //设置四个电机的速度
            MecanumKinematics(0,0,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(0,0);
            break;
    }


}

void ChassisRotate(Chassis_Direction_e Direction,float Velocity,float Angle)
{

}
