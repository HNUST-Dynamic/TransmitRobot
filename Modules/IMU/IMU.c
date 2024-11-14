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

float Yaw_current;
float Yaw_base;
volatile float Yaw_t = 0;
static uint8_t flag = 0;

uint8_t tail[4]={0x00, 0x00, 0x80, 0x7f};
uint8_t ch[8]; //u8是单个字节的数据
USARTInstance* USARTInstance5;
void Serial_SendFloat(float data)
{		

	float x = data;
    ch[0]=*((uint8_t*)&x);  //取出x的地址，强制转换为u8类型，赋值给ch[0]，就可以取出最低位的数值了
    ch[1]=*((uint8_t*)&x+1);  //x的地址加一就是次低位
    ch[2]=*((uint8_t*)&x+2);  //。。。
    ch[3]=*((uint8_t*)&x+3);  //最高位
    ch[4]=0x00;
    ch[5]=0x00;
    ch[6]=0x80;
    ch[7]=0x7f;
    USARTSend(USARTInstance5,ch,8,USART_TRANSFER_DMA);
    //USARTSend(USARTInstance5,tail,4,USART_TRANSFER_DMA);
}

void IMUInit()
{
    USART_Init_Config_s USART_Init_Config = {
        .usart_handle = &huart5,
    };
    USARTInstance5 = USARTRegister(&USART_Init_Config);
    iic_init();
    iic_start();
    iic_send_byte(0xA0);
    while(iic_wait_ack());
    iic_send_byte(0x69);
    while(iic_wait_ack());
    iic_send_byte(0x88);
    while(iic_wait_ack());
    iic_send_byte(0xB5);
    while(iic_wait_ack());
    iic_stop();

    HAL_Delay(200);
    iic_start();
    iic_send_byte(0xA0);
    while(iic_wait_ack());
    iic_send_byte(0x01);
    while(iic_wait_ack());
    iic_send_byte(0x04);
    while(iic_wait_ack());
    iic_send_byte(0x00);
    while(iic_wait_ack());
    iic_stop();
    HAL_Delay(3000);
    iic_start();
    iic_send_byte(0xA0);
    while(iic_wait_ack());
    iic_send_byte(0x00);
    while(iic_wait_ack());
    iic_send_byte(0x00);
    while(iic_wait_ack());
    iic_send_byte(0x00);
    while(iic_wait_ack());
    iic_stop();
    HAL_Delay(200);
    //检查初始值
    uint8_t ReciveData[2];

    iic_start();
    iic_send_byte(0xA0);
    while(iic_wait_ack());
    iic_send_byte(0x3f);
    while(iic_wait_ack());
    iic_start();
    iic_send_byte(0xA1);
    while(iic_wait_ack());

    HAL_Delay(2);
    ReciveData[0] = iic_read_byte(1);
    ReciveData[1] = iic_read_byte(0);
    iic_stop();

    Yaw_base = (((float)((ReciveData[1]<<8)|ReciveData[0])/32768.0)*180.0);

}
void IMURecive()
{
    uint8_t ReciveData[2];

    iic_start();
    iic_send_byte(0xA0);
    while(iic_wait_ack());
    iic_send_byte(0x3f);
    while(iic_wait_ack());
    iic_start();
    iic_send_byte(0xA1);
    while(iic_wait_ack());

    HAL_Delay(2);
    ReciveData[0] = iic_read_byte(1);
    ReciveData[1] = iic_read_byte(0);
    iic_stop();

    Yaw_current = (((float)((ReciveData[1]<<8)|ReciveData[0])/32768.0)*180.0);
    Yaw_t = Yaw_current - Yaw_base;
    if((Yaw_current - Yaw_base) <= -140.0)
    {
        Yaw_t = 360 - Yaw_base + Yaw_current;
    }else if((Yaw_current - Yaw_base) >= 250.0){
        Yaw_t = (0 - Yaw_base) + (Yaw_current - 360);
    }
    //Yaw_base=0;
    //return Yaw_t;
    //Serial_SendFloat(fabs(Yaw_t));//todo：完善串口上位机，添加可视化负浮点数的功能

}
