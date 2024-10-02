#ifndef LIFT_H
#define LIFT_H
#include "Chassis.h"
void Lift_Init();
void pickup();
void putdown();
void Lift_Turn();
void Lift_Turn_back();
void TurnTabble_Turn();

void ElevatorMotor_Init();
void Lift_updown_control(Chassis_Direction_e Direction,float Velocity,float Length);
void Lift_StartFirst();
void Lift_wholeProcess();
#endif // !LIFT_H
#define LIFT_H