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

void IMU_Init()
{
    IIC_Init_Config_s IIC_Init_Config = {
        .id = 0,
        .work_mode = IIC_BLOCK_MODE,
        .dev_address = 0x51,
        .callback = &IICCallback,
        .handle = &hi2c1,
    };

    IIC1 = IICRegister(&IIC_Init_Config);

}

void IMU_Send(IICInstance* IIC)
{
    uint8_t Data[2] = {0x3f,(IIC->dev_address<<1)|1};
    IICTransmit(IIC1,&Data,sizeof(Data),IIC_SEQ_RELEASE);
}

void IMU_Recive(IICInstance* IIC)
{
    IICReceive(IIC1,&ReciveData,sizeof(ReciveData),IIC_SEQ_RELEASE);
}
