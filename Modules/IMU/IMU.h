#ifndef IMU_H
#define IMU_H

#include "main.h"
#include "bsp_iic.h"

extern uint8_t ReciveData[2];

/**
 * @brief IMU初始化
 * 
 */
void IMUInit();

/**
 * @brief IMU发送读取请求
 * 
 */
void IMUSend();

/**
 * @brief IMU接受返回数据
 * 
 * @return void 
 */
void IMURecive();

#endif