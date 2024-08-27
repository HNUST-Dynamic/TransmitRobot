#ifndef IMU_H
#define IMU_H

#include "main.h"
#include "bsp_iic.h"

/**
 * @brief IMU初始化
 * 
 */
void IMU_Init();

/**
 * @brief IMU发送读取请求
 * 
 */
void IMU_Send(IICInstance* IIC);

/**
 * @brief IMU接受返回数据
 * 
 * @return void 
 */
void IMU_Recive(IICInstance* IIC);

#endif