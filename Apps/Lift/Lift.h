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
void Lift_Init();
void pickup();
void putdown();
void Lift_Turn();
void Lift_Turn_back();
void TurnTabble_Turn();
void angle_tset();
void Turn_Red();
void Turn_Blue();
void Turn_Green();
void Turn_Color(uint8_t element);
void ElevatorMotor_Init();
void Lift_updown_control(Lift_Direction_e Direction,uint16_t Velocity,uint32_t Length);
void Lift_StartFirst();
void Lift_Catch(uint8_t element);
void Lift_Back();
 void Goods_Putdown(uint8_t element);


#endif // !LIFT_H
#define LIFT_H