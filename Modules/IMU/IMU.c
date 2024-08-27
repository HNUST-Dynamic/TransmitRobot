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

uint8_t ReciveData[2];
IICInstance* IIC1;

void IICCallback()
{

}

void IMUInit()
{
    IIC_Init_Config_s IIC_Init_Config = {
        .id = 0,
        .work_mode = IIC_BLOCK_MODE,
        .dev_address = 0x50,
        .callback = &IICCallback,
        .handle = &hi2c2,
    };

    IIC1 = IICRegister(&IIC_Init_Config);

}

void IMUSend()
{
    uint8_t Data = 0x3f;
    IICTransmit(IIC1,&Data,1,IIC_SEQ_RELEASE);
}

void IMURecive()
{
    IIC1->dev_address = 0x51;
    IICReceive(IIC1,&ReciveData,1,IIC_SEQ_RELEASE);
}
