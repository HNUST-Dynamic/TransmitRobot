#ifndef __OLED_H
#define __OLED_H 

#include "stm32f4xx_hal.h"

#define USE_HORIZONTAL 0  //������ʾ���� 0��������ʾ��1����ת180����ʾ

//-----------------����LED�˿ڶ���---------------- 

#define LED_ON       HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12, GPIO_PIN_RESET)
#define LED_OFF      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12, GPIO_PIN_SET)

//-----------------OLED�˿ڶ���---------------- 

#define OLED_SCL_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6, GPIO_PIN_RESET)//SCL
#define OLED_SCL_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6, GPIO_PIN_SET)

#define OLED_SDA_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7, GPIO_PIN_RESET)//SDA
#define OLED_SDA_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7, GPIO_PIN_SET)

#define OLED_RES_Clr() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2, GPIO_PIN_RESET)//RES
#define OLED_RES_Set() HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2, GPIO_PIN_SET)


#define u8 unsigned char 
#define u16 unsigned int
#define u32 unsigned long
#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

void OLED_WR_REG(u8 reg);//д��һ��ָ��
void OLED_WR_Byte(u8 dat);//д��һ������
void Column_Address(u8 a,u8 b);//�����е�ַ
void Row_Address(u8 a,u8 b);//�����е�ַ
void OLED_Fill(u16 xstr,u16 ystr,u16 xend,u16 yend,u8 color);//��亯��?
void OLED_ShowChinese(u8 x,u8 y,u8 *s,u8 sizey,u8 mode);//��ʾ���ִ�
void OLED_ShowChinese16x16(u8 x,u8 y,u8 *s,u8 sizey,u8 mode);//��ʾ16x16����
void OLED_ShowChinese24x24(u8 x,u8 y,u8 *s,u8 sizey,u8 mode);//��ʾ24x24����
void OLED_ShowChinese32x32(u8 x,u8 y,u8 *s,u8 sizey,u8 mode);//��ʾ32x32����
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 sizey,u8 mode);//��ʾ�ַ�
void OLED_ShowString(u8 x,u8 y,u8 *dp,u8 sizey,u8 mode);//��ʾ�ַ���
u32 oled_pow(u8 m,u8 n);//�ݺ���
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 sizey,u8 mode);//��ʾ��������
void OLED_DrawBMP(u8 x,u8 y,u16 length,u8 width,const u8 BMP[],u8 mode);//��ʾ�Ҷ�ͼƬ
void OLED_DrawSingleBMP(u8 x,u8 y,u16 length,u8 width,const u8 BMP[],u8 mode);//��ʾ��ɫͼƬ
void OLED_Init(void);
#endif

