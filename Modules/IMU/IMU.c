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


USARTInstance* USARTInstance5;
void Serial_SendFloat(float data)
{		

	float x = data;
    static uint8_t tail[4]={0x00, 0x00, 0x80, 0x7f};
    static uint8_t ch[4]; //u8是单个字节的数据
    ch[0]=*((uint8_t*)&x);  //取出x的地址，强制转换为u8类型，赋值给ch[0]，就可以取出最低位的数值了
    ch[1]=*((uint8_t*)&x+1);  //x的地址加一就是次低位
    ch[2]=*((uint8_t*)&x+2);  //。。。
    ch[3]=*((uint8_t*)&x+3);  //最高位
    USARTSend(USARTInstance5,ch,4,USART_TRANSFER_DMA);
    HAL_Delay(100);
    // if(USARTIsReady(USARTInstance5))
    USARTSend(USARTInstance5,tail,4,USART_TRANSFER_DMA);
}
void IMUInit()
{
    USART_Init_Config_s USART_Init_Config = {
        .usart_handle = &huart5,
    };
    USARTInstance5 = USARTRegister(&USART_Init_Config);

}
void IMURecive()
{
    uint8_t ReciveData[2];
    float Yaw;
    float Yaw_t = 3.1;

    iic_init();
    iic_start();
    iic_send_byte(0xA0);
    while(iic_wait_ack());
    iic_send_byte(0x3f);
    while(iic_wait_ack());
    iic_start();
    iic_send_byte(0xA1);
    while(iic_wait_ack());

    ReciveData[0] = iic_read_byte(1);
    ReciveData[1] = iic_read_byte(0);
    iic_stop();

    Yaw = (((float)((ReciveData[1]<<8)|ReciveData[0])/32768.0)*180.0);
    Serial_SendFloat(Yaw);

}
