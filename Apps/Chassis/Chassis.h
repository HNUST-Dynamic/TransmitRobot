#ifndef CHASSIS_H
#define CHASSIS_H

#include <stdint.h>
#include "StepMotor.h"
// 底盘参数
#define WHEEL_RADIUS        0.0375f     // 轮子半径（米）
#define ROBOT_RADIUS        0.15f       // 中心到轮子的距离（米）
#define PI                  3.1415926f  // 圆周率

#define ROTATION_TIME       800         // 底盘旋转时间系数
#define K                   8.5625      // 平移长度系数
#define MU                  0.876       // 场地摩擦系数 越大走的越远
#define T                   200         // 命令等待时间系数


typedef enum
{
    Forward,
    Back,
    Left,
    Right,
    LeftForward,
    RightForward,
    LeftBack,
    RightBack,
    ClockWise_Chassis,
    CounterClockWise_Chassis
} Chassis_Direction_e;                  // 底盘运动方向枚举

/**
 * @brief 底盘应用初始化 (目前会被RobotInit()调用)
 * 
 */
void ChassisInit();

/**
 * @brief 底盘平移
 * 
 * @param Direction 方向
 * @param Velocity  速度
 * @param Length    距离(绝对值)
 */
void ChassisTransiation(Chassis_Direction_e Direction,uint16_t Velocity,uint32_t Length);

/**
 * @brief 底盘旋转
 * 
 * @param Direction 方向
 * @param Velocity  速度
 * @param Angle     角度
 * @param omega     旋转角速度
 */
void ChassisRotate(Chassis_Direction_e Direction,uint16_t Velocity,float Angle);

/**
 * @brief 底盘停止
 * 
 */
void ChassisStop();

void ChassisTransiation_Adapt(Chassis_Direction_e Direction, uint16_t Velocity, uint32_t clk);

#endif // !CHASSIS_H
