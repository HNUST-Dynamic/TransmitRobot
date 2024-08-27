#ifndef IMU_H
#define IMU_H

#include "main.h"

/**
 * @brief IMU发送读取请求
 * 
 */
void IMU_Send();

/**
 * @brief IMU接受返回数据
 * 
 * @return float 
 */
float IMU_Recive();

#endif