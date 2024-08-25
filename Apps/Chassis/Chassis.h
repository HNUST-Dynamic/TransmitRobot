#ifndef CHASSIS_H
#define CHASSIS_H

#include <stdint.h>
// 底盘参数
#define WHEEL_RADIUS 0.0375f // 轮子半径（米）
#define ROBOT_RADIUS 0.15f   // 中心到轮子的距离（米）
#define PI           3.1415926f

#define ROTATION_TIME 800
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
} Chassis_Direction_e;

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
void ChassisTransiation(Chassis_Direction_e Direction,float Velocity,float Length);

/**
 * @brief 底盘旋转
 * 
 * @param Direction 方向
 * @param Velocity  速度
 * @param Angle     角度
 * @param omega     旋转角速度
 */
void ChassisRotate(Chassis_Direction_e Direction,float Velocity,float Angle,float omega);

#endif // !CHASSIS_H