#ifndef LIFT_H
#define LIFT_H
#include "ServoMotor.h"
extern ServoInstance *GripperServoMotor_Instance, // 抓手舵机
    *ElevatorServoMotor_Instance,                 // 电梯下盘舵机
    *TurntableServoMotor_Instance;                // 物料盘舵机

extern TIM_OC_InitTypeDef sConfigOC;
typedef enum
{
    up,
    down
} Lift_Direction_e;

/**
 * @brief 电梯机构三个舵机初始化
 *
 */
void Lift_Init();

/**
 * @brief 抓手抓取
 *
 */
void pickup();

/**
 * @brief 抓手松开
 *
 */
void putdown();

/**
 * @brief 电梯由内转向外
 *
 */
void Lift_Turn();

/**
 * @brief 电梯由外转向内
 *
 */
void Lift_Turn_back();

/**
 * @brief 根据传入的颜色将转盘转到对应角度
 *
 * @param element 传入的颜色代号
 */
void Turn_Color(uint8_t element);

/**
 * @brief 到加工区时根据传入的颜色将转盘转到对应角度，
 *          实际上可以和Turn_Color函数合并，宋元昊的锅
 *
 * @param element 传入的颜色代号
 */
void Turn_Color_two(uint8_t element);

/**
 * @brief 电梯步进电机初始化
 *
 */
void ElevatorMotor_Init();

/**
 * @brief 电梯上下控制
 *
 * @param Direction 方向
 * @param Velocity 速度
 * @param Length 距离
 */
void Lift_updown_control(Lift_Direction_e Direction, uint16_t Velocity, uint32_t Length);

/**
 * @brief 电机初始展开
 *
 */
void Lift_StartFirst();

/**
 * @brief 抓取物料，电梯升，转向，物料盘动，放下
 *
 * @param element 抓取的物料
 * @note 不用想了，这种离谱的代码肯定是宋元昊写的
 */
void Lift_Catch(uint8_t element);

/**
 * @brief 放完物料后，电梯归位到下一步抓取待机状态
 *
 */
void Lift_Back();

/**
 * @brief 粗加工区发下物料
 *
 * @param element 物料代码
 */
void Goods_Putdown(uint8_t element);

/**
 * @brief 暂存区抓取物料
 *
 * @param element 物料代码
 * @note 又是宋元昊写的“好代码”
 */
void Lift_Catch_two(uint8_t element);

/**
 * @brief 暂存区暂存区抓取物料
 *
 * @param element 物料代码
 */
void Goods_Pickup(uint8_t element);


#endif // !LIFT_H
