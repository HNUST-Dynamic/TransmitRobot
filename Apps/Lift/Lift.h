#ifndef LIFT_H
#define LIFT_H
#include "ServoMotor.h"
extern ServoInstance     *GripperServoMotor_Instance,    // 抓手舵机
                          *ElevatorServoMotor_Instance,   //电梯下盘舵机
                          *TurntableServoMotor_Instance;  //物料盘舵机

extern TIM_OC_InitTypeDef sConfigOC;
typedef enum
{
    up,
    down
} Lift_Direction_e;
/** 初始化 */
void Lift_Init();
/** 抓手抓取 */
void pickup();
/** 抓手松开 */
void putdown();
/** 电梯由内转外 */
void Lift_Turn();
/** 电梯由外转内 */
void Lift_Turn_back();
/**这个不用了 */
void TurnTabble_Turn();
/**不用了 */
void angle_tset();
/** 物料盘中将红色存放区转到待取区 */
void Turn_Red();
/** 物料盘中将蓝色存放区转到待取区 */
void Turn_Blue();
/**物料盘中将绿色存放区转到待取区  */
void Turn_Green();
/** 根据视觉传回，将对应的转到待取区 */
void Turn_Color(uint8_t element);
/** 当到加工区的物料盘转动 */
void Turn_Color_two(uint8_t element);
/** 电梯步进电机初始化 */
void ElevatorMotor_Init();
/** 电梯上下控制，方向（up，down），速度，距离 */
void Lift_updown_control(Lift_Direction_e Direction,uint16_t Velocity,uint32_t Length);
/** 电机初始展开 */
void Lift_StartFirst();
/** 抓取物料，电梯升，转向，物料盘动，放下 */
void Lift_Catch(uint8_t element);
/** 放完物料后，电梯归位到下一步抓取待机状态 */
void Lift_Back();
/** 粗加工区发下物料 */
 void Goods_Putdown(uint8_t element);
/*暂存区抓取物料*/
void Lift_Catch_two(uint8_t element);
/*暂存区暂存区抓取物料*/
void Goods_Pickup(uint8_t element);

#endif // !LIFT_H
#define LIFT_H