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
        .step_mode = PosMode,
        .ctrl_mode = CloseCircuit,
        .motor_direction = CounterClockWise,
        .acc = 0,
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

    //电机位置清零
    StepMotorResetZero(LeftForwardMotorInstance);
    StepMotorResetZero(RightForwardMotorInstance);
    StepMotorResetZero(RightBackMotorInstance);
    StepMotorResetZero(LeftBackMotorInstance);
    HAL_Delay(200);
    //电机修改成闭环控制
    StepMotorModifyCtrlMode(LeftForwardMotorInstance,true);
    StepMotorModifyCtrlMode(RightForwardMotorInstance,true);
    StepMotorModifyCtrlMode(RightBackMotorInstance,true);
    StepMotorModifyCtrlMode(LeftBackMotorInstance,true);
    HAL_Delay(200);
    //电机使能
    StepMotorEnControl(LeftForwardMotorInstance,true,false);
    StepMotorEnControl(RightForwardMotorInstance,true,false);
    StepMotorEnControl(RightBackMotorInstance,true,false);
    StepMotorEnControl(LeftBackMotorInstance,true,false);
    HAL_Delay(200);
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
    LeftForwardMotorInstance->speed = (uint16_t)(fabs((vx + vy + (ROBOT_RADIUS * omega)) / WHEEL_RADIUS));  // 左前轮 单位rad/s
    RightForwardMotorInstance->speed = (uint16_t)(fabs((-vx + vy - (ROBOT_RADIUS * omega)) / WHEEL_RADIUS));  // 右前轮
    RightBackMotorInstance->speed = (uint16_t)(fabs((vx + vy - (ROBOT_RADIUS * omega)) / WHEEL_RADIUS));  // 右后轮
    LeftBackMotorInstance->speed = (uint16_t)(fabs((-vx + vy + (ROBOT_RADIUS * omega)) / WHEEL_RADIUS));  // 左后轮

}
/**
 * @brief           运动学逆解算函数
 * 
 * @param distance_x x方向移动的目标距离（米）
 * @param distance_y y方向移动的目标距离（米）
 */
void MecanumInverseKinematics(uint32_t distance_x,uint32_t distance_y)
{
    LeftForwardMotorInstance->clk = (uint32_t)(fabs(((distance_x + distance_y)*180) / (PI*WHEEL_RADIUS)));  // 左前轮 单位rad/s
    RightForwardMotorInstance->clk = (uint32_t)(fabs(((-distance_x + distance_y)*180) / (PI*WHEEL_RADIUS)));  // 右前轮
    RightBackMotorInstance->clk = (uint32_t)(fabs(((distance_x + distance_y)*180) / (PI*WHEEL_RADIUS)));  // 右后轮
    LeftBackMotorInstance->clk = (uint32_t)(fabs(((-distance_x + distance_y)*180) / (PI*WHEEL_RADIUS)));  // 左后轮

}

void ChassisTransiation(Chassis_Direction_e Direction,uint16_t Velocity,uint32_t Length)
{
    //设置四个电机为pos模式
    LeftForwardMotorInstance->step_mode = PosMode;
    RightForwardMotorInstance->step_mode = PosMode;
    RightBackMotorInstance->step_mode = PosMode;
    LeftBackMotorInstance->step_mode = PosMode;

    switch(Direction)
    {
        case Forward:
            //设置四个电机的方向
            LeftForwardMotorInstance->motor_direction = CounterClockWise;
            RightForwardMotorInstance->motor_direction = ClockWise;
            RightBackMotorInstance->motor_direction = ClockWise;
            LeftBackMotorInstance->motor_direction = CounterClockWise;
            //设置四个电机的速度
            MecanumKinematics(0,Velocity,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(0,Length);
            break;
        case Back:
            //设置四个电机的方向
            LeftForwardMotorInstance->motor_direction = ClockWise;
            RightForwardMotorInstance->motor_direction = CounterClockWise;
            RightBackMotorInstance->motor_direction = CounterClockWise;
            LeftBackMotorInstance->motor_direction = ClockWise;
            //设置四个电机的速度
            MecanumKinematics(Velocity,0,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(Length,0);
            break;
        case Left:
            //设置四个电机的方向
            LeftForwardMotorInstance->motor_direction = CounterClockWise;
            RightForwardMotorInstance->motor_direction = CounterClockWise;
            RightBackMotorInstance->motor_direction = ClockWise;
            LeftBackMotorInstance->motor_direction = ClockWise;
            //设置四个电机的速度
            MecanumKinematics(Velocity,0,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(Length,0);
            break;
        case Right:
            //设置四个电机的方向
            LeftForwardMotorInstance->motor_direction = ClockWise;
            RightForwardMotorInstance->motor_direction = ClockWise;
            RightBackMotorInstance->motor_direction = CounterClockWise;
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
            RightBackMotorInstance->motor_direction = ClockWise;
            LeftBackMotorInstance->motor_direction = CounterClockWise;
            //设置四个电机的速度
            MecanumKinematics(0.707*Velocity,0.707*Velocity,0);
            //设置四个电机转动角度
            MecanumInverseKinematics(0.707*Length,0.707*Length);
            break;
        case RightForward:
            //设置四个电机的方向
            LeftForwardMotorInstance->motor_direction = CounterClockWise;
            RightForwardMotorInstance->motor_direction = ClockWise;
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
            RightForwardMotorInstance->motor_direction = CounterClockWise;
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
            RightBackMotorInstance->motor_direction = CounterClockWise;
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

    //确保电机使能
    StepMotorEnControl(LeftForwardMotorInstance,true,false);
    StepMotorEnControl(RightForwardMotorInstance,true,false);
    StepMotorEnControl(RightBackMotorInstance,true,false);
    StepMotorEnControl(LeftBackMotorInstance,true,false);
    HAL_Delay(200);
    //电机位置模式运行
    StepMotorPosControl(LeftForwardMotorInstance,false,false);
    StepMotorPosControl(RightForwardMotorInstance,false,false);
    StepMotorPosControl(RightBackMotorInstance,false,false);
    StepMotorPosControl(LeftBackMotorInstance,false,false);

}
void RotationMecanumInverseKinematics(Chassis_Direction_e Direction,float Angle)
{
     // 计算每个轮子需要转动的距离（假设角度为正时顺时针旋转）
    float rotation_circumference = 2 * PI * ROBOT_RADIUS;
    float distance_per_degree = rotation_circumference / 360.0;
    float distance = distance_per_degree * Angle;

    // 计算每个轮子的转动角度
    float wheel_rotation_angle = (distance / WHEEL_RADIUS) * (180.0 / PI); // 转换为度数

    // // 将角度转换为电机的步数或角度
    // float motor_rotation = wheel_rotation_angle * MOTOR_GEAR_RATIO;
    // 将旋转角度转换为步数
    float steps_per_degree = (STEPS_PER_REVOLUTION * MICROSTEPS) / 360.0;
    uint32_t steps = (uint32_t)(wheel_rotation_angle * steps_per_degree);

    // 设置每个电机脉冲
    if(Direction == ClockWise_Chassis)
    {
        LeftForwardMotorInstance->motor_direction = CounterClockWise;
        LeftForwardMotorInstance->clk = (uint32_t)(fabs(steps));  // 左前轮

        RightForwardMotorInstance->motor_direction = ClockWise;
        RightForwardMotorInstance->clk = (uint32_t)(fabs(steps));  // 右前

        RightBackMotorInstance->motor_direction = ClockWise;
        RightBackMotorInstance->clk = (uint32_t)(fabs(steps));  // 右后轮

        LeftBackMotorInstance->motor_direction = CounterClockWise;
        LeftBackMotorInstance->clk = (uint32_t)(fabs(steps));  // 左后轮
    }else{
        LeftForwardMotorInstance->motor_direction = ClockWise;
        LeftForwardMotorInstance->clk = (uint32_t)(fabs(steps));  // 左前轮

        RightForwardMotorInstance->motor_direction = CounterClockWise;
        RightForwardMotorInstance->clk = (uint32_t)(fabs(steps));  // 右前

        RightBackMotorInstance->motor_direction = CounterClockWise;
        RightBackMotorInstance->clk = (uint32_t)(fabs(steps));  // 右后轮

        LeftBackMotorInstance->motor_direction = ClockWise;
        LeftBackMotorInstance->clk = (uint32_t)(fabs(steps));  // 左后轮
    }

}
void ChassisRotate(Chassis_Direction_e Direction,float Velocity,float Angle)
{

    #ifdef USE_IMU

    /*…………预留IMU结合算法…………*/

    #endif

    #ifndef USE_IMU
    //底盘旋转运动学逆解算
    RotationMecanumInverseKinematics(Direction,Angle);

    //确保电机使能
    StepMotorEnControl(LeftForwardMotorInstance,true,true);
    StepMotorEnControl(RightForwardMotorInstance,true,true);
    StepMotorEnControl(RightBackMotorInstance,true,true);
    StepMotorEnControl(LeftBackMotorInstance,true,true);

    //电机位置模式运行
    StepMotorPosControl(LeftForwardMotorInstance,false,true);
    StepMotorPosControl(RightForwardMotorInstance,false,true);
    StepMotorPosControl(RightBackMotorInstance,false,true);
    StepMotorPosControl(LeftBackMotorInstance,false,true);

    //预留执行时间
    HAL_Delay(ROTATION_TIME);

    //到时急停
    StepMotorStop(LeftForwardMotorInstance,true);
    StepMotorStop(RightForwardMotorInstance,true);
    StepMotorStop(RightBackMotorInstance,true);
    StepMotorStop(LeftBackMotorInstance,true);


    #endif // !USE_IMU USE_IMU

}
