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

static uint8_t speed[4];
static uint8_t data[4];

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
        .control_mode = PosMode,
        .motor_direction = CounterClockWise,
        .subdivision = 0x20,
        .data = 300,
        .speed = 10,
        .control = &StepMotorControl,
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

    // LeftForwardMotorInstance->control(LeftForwardMotorInstance);
    // RightForwardMotorInstance->control(RightForwardMotorInstance);
    // RightBackMotorInstance->control(RightBackMotorInstance);
    // LeftBackMotorInstance->control(LeftBackMotorInstance);

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
    // 计算每个轮子的转速
    LeftForwardMotorInstance->speed = (uint8_t)(abs((vx + vy - (ROBOT_RADIUS * omega)) / WHEEL_RADIUS));  // 左前轮 单位rad/s
    RightForwardMotorInstance->speed = (uint8_t)(abs((-vx + vy + (ROBOT_RADIUS * omega)) / WHEEL_RADIUS));  // 右前轮
    RightBackMotorInstance->speed = (uint8_t)(abs((vx + vy + (ROBOT_RADIUS * omega)) / WHEEL_RADIUS));  // 右后轮
    LeftBackMotorInstance->speed = (uint8_t)(abs((-vx + vy - (ROBOT_RADIUS * omega)) / WHEEL_RADIUS));  // 左后轮
    // speed[0] = (uint8_t)(abs((vx + vy - (ROBOT_RADIUS * omega)) / WHEEL_RADIUS));  // 左前轮 单位rad/s
    // speed[1] = (uint8_t)(abs((-vx + vy + (ROBOT_RADIUS * omega)) / WHEEL_RADIUS));  // 右前轮
    // speed[2] = (uint8_t)(abs((vx + vy + (ROBOT_RADIUS * omega)) / WHEEL_RADIUS));  // 右后轮
    // speed[3] = (uint8_t)(abs((-vx + vy - (ROBOT_RADIUS * omega)) / WHEEL_RADIUS));  // 左后轮

}
/**
 * @brief           运动学逆解算函数
 * 
 * @param distance_x x方向移动的目标距离（米）
 * @param distance_y y方向移动的目标距离（米）
 */
void MecanumInverseKinematics(float distance_x,float distance_y)
{
    LeftForwardMotorInstance->data = (uint8_t)(abs(((distance_x + distance_y)*180) / (PI*WHEEL_RADIUS)));  // 左前轮 单位rad/s
    RightForwardMotorInstance->data = (uint8_t)(abs(((-distance_x + distance_y)*180) / (PI*WHEEL_RADIUS)));  // 右前轮
    RightBackMotorInstance->data = (uint8_t)(abs(((distance_x + distance_y)*180) / (PI*WHEEL_RADIUS)));  // 右后轮
    LeftBackMotorInstance->data = (uint8_t)(abs(((-distance_x + distance_y)*180) / (PI*WHEEL_RADIUS)));  // 左后轮
    // data[0] = (uint8_t)(abs(((distance_x + distance_y)*180) / (PI*WHEEL_RADIUS)));  // 左前轮 单位rad/s
    // data[1] = (uint8_t)(abs(((-distance_x + distance_y)*180) / (PI*WHEEL_RADIUS)));  // 右前轮
    // data[2] = (uint8_t)(abs(((distance_x + distance_y)*180) / (PI*WHEEL_RADIUS)));  // 右后轮
    // data[3] = (uint8_t)(abs(((-distance_x + distance_y)*180) / (PI*WHEEL_RADIUS)));  // 左后轮

}

void ChassisTransiation(Chassis_Direction_e Direction,float Velocity,float Length)
{
    //设置四个电机为pos模式
    LeftForwardMotorInstance->control_mode = PosMode;
    RightForwardMotorInstance->control_mode = PosMode;
    RightBackMotorInstance->control_mode = PosMode;
    LeftBackMotorInstance->control_mode = PosMode;

    switch(Direction){
        case Forward:
            //设置四个电机的速度
            MecanumKinematics(0,Velocity,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(0,Length);
            break;
        case Back:
            //设置四个电机的速度
            MecanumKinematics(Velocity,0,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(Length,0);
            break;
        case Left:
            //设置四个电机的速度
            MecanumKinematics(Velocity,0,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(Length,0);
            break;
        case Right:
            //设置四个电机的速度
            MecanumKinematics(-Velocity,0,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(Length,0);
            break;
        case LeftForward:
            //设置四个电机的速度
            MecanumKinematics(0.707*Velocity,0.707*Velocity,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(0.707*Length,0.707*Length);
            break;
        case RightForward:
            //设置四个电机的速度
            MecanumKinematics(-0.707*Velocity,0.707*Velocity,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(0.707*Length,0.707*Length);
            break;
        case LeftBack:
            //设置四个电机的速度
            MecanumKinematics(0.707*Velocity,-0.707*Velocity,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(0.707*Length,0.707*Length);
            break;
        case RightBack:
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

    //启动四个步进电机
    LeftForwardMotorInstance->control(LeftForwardMotorInstance);
    RightForwardMotorInstance->control(RightForwardMotorInstance);
    RightBackMotorInstance->control(RightBackMotorInstance);
    LeftBackMotorInstance->control(LeftBackMotorInstance);

}

void ChassisRotate(Chassis_Direction_e Direction,float Velocity,float Angle)
{

}
