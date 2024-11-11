#ifndef IMU_H
#define IMU_H

#include "main.h"
#include "bsp_usart.h"
#include "i2c.h"
extern float Yaw_t;
void IMUInit();

void IMURecive();


#endif