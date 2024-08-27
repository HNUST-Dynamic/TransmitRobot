/**
 * @file IMU.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "IMU.h"

void IMU_Send()
{
    HAL_I2C_Mem_Write(hi2c2,0x51,0x3f,1,&Data,1,100);
}
