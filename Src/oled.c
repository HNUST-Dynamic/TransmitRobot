#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"

//��ʱ
void IIC_delay(void)
{
	u8 t=3;
	while(t--);
}

//��ʼ�ź�
void I2C_Start(void)
{
	OLED_SDA_Set();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SDA_Clr();
	IIC_delay();
	OLED_SCL_Clr();
	 
}

//�����ź�
void I2C_Stop(void)
{
	OLED_SDA_Clr();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SDA_Set();
}

//�ȴ��ź���Ӧ
void I2C_WaitAck(void) //�������źŵĵ�ƽ
{
	OLED_SDA_Set();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SCL_Clr();
}

//д��һ���ֽ�
void Send_Byte(u8 dat)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		OLED_SCL_Clr();//��ʱ���ź�����Ϊ�͵�ƽ
		if(dat&0x80)//��dat��8λ�����λ����д��
		{
			OLED_SDA_Set();
    }
		else
		{
			OLED_SDA_Clr();
    }
		IIC_delay();
		OLED_SCL_Set();
		IIC_delay();
		OLED_SCL_Clr();
		dat<<=1;
  }
}

//******************************************************************************
//    ����˵����OLEDд��һ��ָ��
//    ������ݣ�reg ָ��
//    ����ֵ��  ��
//******************************************************************************
void OLED_WR_REG(u8 reg)
{
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	Send_Byte(0x00);
	I2C_WaitAck();
	Send_Byte(reg);
	I2C_WaitAck();
	I2C_Stop();	
}
//******************************************************************************
//    ����˵����OLEDд��һ������
//    ������ݣ�dat ����
//    ����ֵ��  ��
//******************************************************************************
void OLED_WR_Byte(u8 dat)
{
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	Send_Byte(0x40);
	I2C_WaitAck();
	Send_Byte(dat);
	I2C_WaitAck();
	I2C_Stop();	
}
//******************************************************************************
//    ����˵����OLED��ʾ�е���ʼ��ֹ��ַ
//    ������ݣ�a  �е���ʼ��ַ
//              b  �е���ֹ��ַ
//    ����ֵ��  ��
//******************************************************************************
void Column_Address(u8 a,u8 b)
{
	OLED_WR_REG(0x15);       // Set Column Address
	OLED_WR_Byte(a+0x08);
	OLED_WR_Byte(b+0x08);
}


//******************************************************************************
//    ����˵����OLED��ʾ�е���ʼ��ֹ��ַ
//    ������ݣ�a  �е���ʼ��ַ
//              b  �е���ֹ��ַ
//    ����ֵ��  ��
//******************************************************************************
void Row_Address(u8 a,u8 b)
{
	OLED_WR_REG(0x75);       // Row Column Address
	OLED_WR_Byte(a);
	OLED_WR_Byte(b);
	OLED_WR_REG(0x5C);    //дRAM����
}

void OLED_Fill(u16 xstr,u16 ystr,u16 xend,u16 yend,u8 color)
{
	u8 x,y;
	xstr/=4; //column address ����4�� 
	xend/=4;
	Column_Address(xstr,xend-1);
	Row_Address(ystr,yend-1);
	for(x=xstr;x<xend;x++)
	{
		for(y=ystr;y<yend;y++)
		{
			OLED_WR_Byte(color); //����4�� ͬʱ֧��16�׻Ҷ� ������Ҫ2���ֽ�
			OLED_WR_Byte(color);
    }
  }
}

//******************************************************************************
//    ����˵����OLED��ʾ����
//    ������ݣ�x,y :�������
//              *s  :Ҫ��ʾ�ĺ��ִ�
//              sizey �ַ��߶� 
//              mode  0:������ʾ��1����ɫ��ʾ
//    ����ֵ��  ��
//******************************************************************************
void OLED_ShowChinese(u8 x,u8 y,u8 *s,u8 sizey,u8 mode)
{
	while(*s!=0)
	{
		if(sizey==16) OLED_ShowChinese16x16(x,y,s,sizey,mode);
		else if(sizey==24) OLED_ShowChinese24x24(x,y,s,sizey,mode);
		else if(sizey==32) OLED_ShowChinese32x32(x,y,s,sizey,mode);
		else return;
		s+=2;
		x+=sizey;
	}
}

//******************************************************************************
//    ����˵����OLED��ʾ����
//    ������ݣ�x,y :�������
//              *s  :Ҫ��ʾ�ĺ���
//              sizey �ַ��߶� 
//              mode  0:������ʾ��1����ɫ��ʾ
//    ����ֵ��  ��
//******************************************************************************

void OLED_ShowChinese16x16(u8 x,u8 y,u8 *s,u8 sizey,u8 mode)
{
	u8 i,j,k,DATA1=0,DATA2=0;
	u16 HZnum;//������Ŀ
	u16 TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;//�ַ���ռ�ֽ���
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//ͳ�ƺ��ֿ���Ŀ
	Column_Address(x/4,x/4+sizey/4-1);
	Row_Address(y,y+sizey-1);
	for(k=0;k<HZnum;k++)
	{
		if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
		{ 	
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<2;j++)
				{
					if(tfont16[k].Msk[i]&(0x01<<(j*4)))
					{
						DATA1=0x0F;
					}
					if(tfont16[k].Msk[i]&(0x01<<(j*4+1)))
					{
						DATA1|=0xF0;
					}
					if(tfont16[k].Msk[i]&(0x01<<(j*4+2)))
					{
						DATA2=0x0F;
					}
					if(tfont16[k].Msk[i]&(0x01<<(j*4+3)))
					{
						DATA2|=0xF0;
					}
					if(mode)
					{
						OLED_WR_Byte(~DATA2);
						OLED_WR_Byte(~DATA1);		
					}
					else
					{	
						OLED_WR_Byte(DATA2);
						OLED_WR_Byte(DATA1);
					}
					DATA1=0;
					DATA2=0;
				}
			}
		}				  	
		continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}
} 

//******************************************************************************
//    ����˵����OLED��ʾ����
//    ������ݣ�x,y :�������
//              *s  :Ҫ��ʾ�ĺ���
//              sizey �ַ��߶� 
//              mode  0:������ʾ��1����ɫ��ʾ
//    ����ֵ��  ��
//******************************************************************************

void OLED_ShowChinese24x24(u8 x,u8 y,u8 *s,u8 sizey,u8 mode)
{
	u8 i,j,k,DATA1=0,DATA2=0;
	u16 HZnum;//������Ŀ
	u16 TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;//�ַ���ռ�ֽ���
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//ͳ�ƺ��ֿ���Ŀ
	Column_Address(x/4,x/4+sizey/4-1);
	Row_Address(y,y+sizey-1);
	for(k=0;k<HZnum;k++)
	{
		if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
		{ 	
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<2;j++)
				{
					if(tfont24[k].Msk[i]&(0x01<<(j*4)))
					{
						DATA1=0x0F;
					}
					if(tfont24[k].Msk[i]&(0x01<<(j*4+1)))
					{
						DATA1|=0xF0;
					}
					if(tfont24[k].Msk[i]&(0x01<<(j*4+2)))
					{
						DATA2=0x0F;
					}
					if(tfont24[k].Msk[i]&(0x01<<(j*4+3)))
					{
						DATA2|=0xF0;
					}
					if(mode)
					{
						OLED_WR_Byte(~DATA2);
						OLED_WR_Byte(~DATA1);		
					}
					else
					{	
						OLED_WR_Byte(DATA2);
						OLED_WR_Byte(DATA1);
					}
					DATA1=0;
					DATA2=0;
				}
			}
		}				  	
		continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}
} 


//******************************************************************************
//    ����˵����OLED��ʾ32x32����
//    ������ݣ�x,y :�������
//              *s  :Ҫ��ʾ�ĺ���
//              sizey �ַ��߶� 
//              mode  0:������ʾ��1����ɫ��ʾ
//    ����ֵ��  ��
//******************************************************************************

void OLED_ShowChinese32x32(u8 x,u8 y,u8 *s,u8 sizey,u8 mode)
{
	u8 i,j,k,DATA1=0,DATA2=0;
	u16 HZnum;//������Ŀ
	u16 TypefaceNum;//һ���ַ���ռ�ֽڴ�С
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;//�ַ���ռ�ֽ���
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//ͳ�ƺ��ֿ���Ŀ
	Column_Address(x/4,x/4+sizey/4-1);
	Row_Address(y,y+sizey-1);
	for(k=0;k<HZnum;k++)
	{
		if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
		{ 	
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<2;j++)
				{
					if(tfont32[k].Msk[i]&(0x01<<(j*4)))
					{
						DATA1=0x0F;
					}
					if(tfont32[k].Msk[i]&(0x01<<(j*4+1)))
					{
						DATA1|=0xF0;
					}
					if(tfont32[k].Msk[i]&(0x01<<(j*4+2)))
					{
						DATA2=0x0F;
					}
					if(tfont32[k].Msk[i]&(0x01<<(j*4+3)))
					{
						DATA2|=0xF0;
					}
					if(mode)
					{
						OLED_WR_Byte(~DATA2);
						OLED_WR_Byte(~DATA1);		
					}
					else
					{	
						OLED_WR_Byte(DATA2);
						OLED_WR_Byte(DATA1);
					}
					DATA1=0;
					DATA2=0;
				}
			}
		}				  	
		continue;  //���ҵ���Ӧ�����ֿ������˳�����ֹ��������ظ�ȡģ����Ӱ��
	}
} 



//******************************************************************************
//    ����˵����OLED��ʾ�ַ����� 
//    �˺������÷�Χ���ַ������4�ı���  �ַ��߶��ǿ�ȵ�2��
//    ������ݣ�x,y   ��ʼ����
//              chr   Ҫд����ַ�
//              sizey �ַ��߶� 
//              mode  0:������ʾ��1����ɫ��ʾ
//    ����ֵ��  ��
//******************************************************************************

void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 sizey,u8 mode)
{
	u8 sizex,c,temp,t=2,DATA1=0,DATA2=0,m;
	u16 i,k,size2;
	x/=4;
	sizex=sizey/4/2; //��4����Ϊһ���е�ַ�߿���4�� ��2���ֿ�:�ָ� 1:2
	size2=(sizey/16+((sizey%16)?1:0))*sizey; //����һ���ַ���ռ�ֽ���
	c=chr-' '; //ASCII��ֵ��Ӧƫ�ƺ��ֵ
	Column_Address(x,x+sizex-1);//�����е�ַ
	Row_Address(y,y+sizey-1);//�����е�ַ
	for(i=0;i<size2;i++)
	{
		if(sizey==16)
		{
			temp=ascii_1608[c][i]; //��ȡȡģ���� 
		}
		else if(sizey==24)
		{
			temp=ascii_2412[c][i];//12x24 ASCII��
		}
		else if(sizey==32)
		{
			temp=ascii_3216[c][i];//16x32 ASCII��
		}
		else if(sizey==40)
		{
			temp=ascii_4024[c][i];//40x20 ASCII��
		}
		if(sizey%16)
		{
			m=sizey/16+1;
			if(i%m) t=1;
			else t=2;
		}
		for(k=0;k<t;k++)
		{
			if(temp&(0x01<<(k*4)))
			{
				DATA1=0x0F;
			}
			if(temp&(0x01<<(k*4+1)))
			{
				DATA1|=0xF0;
			}
			if(temp&(0x01<<(k*4+2)))
			{
				DATA2=0x0F;
			}
			if(temp&(0x01<<(k*4+3)))
			{
				DATA2|=0xF0;
			}
			if(mode)
			{
				OLED_WR_Byte(~DATA2);
				OLED_WR_Byte(~DATA1);		
			}
			else
			{	
				OLED_WR_Byte(DATA2);
				OLED_WR_Byte(DATA1);
			}
			DATA1=0;
			DATA2=0;
		}
	}
}

//******************************************************************************
//    ����˵����OLED��ʾ�ַ���
//    ������ݣ�x,y  ��ʼ����
//              *dp   Ҫд����ַ�
//              sizey �ַ��߶� 
//              mode  0:������ʾ��1����ɫ��ʾ
//    ����ֵ��  ��
//******************************************************************************
void OLED_ShowString(u8 x,u8 y,u8 *dp,u8 sizey,u8 mode)
{
	while(*dp!='\0')
	{
	  OLED_ShowChar(x,y,*dp,sizey,mode);
		dp++;
		x+=sizey/2;
	}
}



//******************************************************************************
//    ����˵����m^n
//    ������ݣ�m:���� n:ָ��
//    ����ֵ��  result
//******************************************************************************
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)result*=m;    
	return result;
}


//******************************************************************************
//    ����˵����OLED��ʾ����
//    ������ݣ�x,y :�������	 
//              num :Ҫ��ʾ�ı���
//              len :���ֵ�λ��
//              sizey �ַ��߶� 
//              mode  0:������ʾ��1����ɫ��ʾ
//    ����ֵ��  ��
//******************************************************************************
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 sizey,u8 mode)
{         	
	u8 t,temp;
	u8 enshow=0;
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(sizey/2)*t,y,' ',sizey,mode);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(sizey/2)*t,y,temp+'0',sizey,mode); 
	}
}

//******************************************************************************
//    ����˵������ʾ�Ҷ�ͼƬ
//    ������ݣ�x,y :�������
//              length ͼƬ����
//              width  ͼƬ���
//              BMP[] :Ҫ��ʾͼƬ
//              mode  0:������ʾ��1����ɫ��ʾ
//    ����ֵ��  ��
//******************************************************************************
void OLED_DrawBMP(u8 x,u8 y,u16 length,u8 width,const u8 BMP[],u8 mode)
{
  u16 i,num;
	length=(length/4+((length%4)?1:0))*4;
	num=length*width/2/2; //����һ���Ҷ�ͼƬ��ռ�ֽ��� һ��д��2���ֽ� �����ٳ���2
	x/=4;
	length/=4;
	Column_Address(x,x+length-1);
	Row_Address(y,y+width-1);
	for(i=0;i<num;i++)
	{
		if(mode)
		{
		 OLED_WR_Byte(~BMP[i*2+1]);
		 OLED_WR_Byte(~BMP[i*2]);
		}
		else
		{
		 OLED_WR_Byte(BMP[i*2+1]);
		 OLED_WR_Byte(BMP[i*2]);			
		}
	}
}

//******************************************************************************
//    ����˵������ʾ��ɫͼƬ
//    ������ݣ�x,y :�������
//              length ͼƬ���� 
//              width  ͼƬ���
//              BMP[] :Ҫ��ʾͼƬ
//              mode  0:������ʾ��1����ɫ��ʾ
//    ����ֵ��  ��
//******************************************************************************
void OLED_DrawSingleBMP(u8 x,u8 y,u16 length,u8 width,const u8 BMP[],u8 mode)
{
	u8 t=2,DATA1=0,DATA2=0;
	u16 i,k,num=0;
	length=(length/8+((length%8)?1:0))*8;
	num=length/8*width;  //����ͼƬ��ռ�ֽ���
	x/=4;
	length/=4;
	Column_Address(x,x+length-1);
	Row_Address(y,y+width-1);
	for(i=0;i<num;i++)
	{
		for(k=0;k<t;k++)
		{
			if(BMP[i]&(0x01<<(k*4)))
			{
				DATA1=0x0F;
			}
			if(BMP[i]&(0x01<<(k*4+1)))
			{
				DATA1|=0xF0;
			}
			if(BMP[i]&(0x01<<(k*4+2)))
			{
				DATA2=0x0F;
			}
			if(BMP[i]&(0x01<<(k*4+3)))
			{
				DATA2|=0xF0;
			}
			if(mode)
			{
				OLED_WR_Byte(~DATA2);
				OLED_WR_Byte(~DATA1);		
			}
			else
			{	
        OLED_WR_Byte(DATA2);
				OLED_WR_Byte(DATA1);				
			}
			DATA1=0;
			DATA2=0;
		}
	}
}

//OLED�ĳ�ʼ��
void OLED_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pins : PA4 PA5 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);

  /*Configure GPIO pin : PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
	
	OLED_RES_Clr();
	HAL_Delay(100);
	OLED_RES_Set();
	HAL_Delay(200);

	OLED_WR_REG(0xfd);  /*Command Lock*/ 
	OLED_WR_Byte(0x12);

	OLED_WR_REG(0xAE);

	OLED_WR_REG(0xC1);  
	OLED_WR_Byte(0xFF);	

	if(USE_HORIZONTAL)
	{
		OLED_WR_REG(0xA0);  
		OLED_WR_Byte(0x24);	
		OLED_WR_Byte(0x00);	

		OLED_WR_REG(0xa2);////A2.Display Offset
		OLED_WR_Byte(0x80); ////Offset:: 0~127				
	}
	else
	{
		OLED_WR_REG(0xA0);  
		OLED_WR_Byte(0x32);	
		OLED_WR_Byte(0x00);	

		OLED_WR_REG(0xa2);////A2.Display Offset
		OLED_WR_Byte(0x20); ////Offset:: 0~127		
	}
	

	OLED_WR_REG(0xca);////CA.Set Mux Ratio
	OLED_WR_Byte(0x7f); ////Mux:: 0~255

	OLED_WR_REG(0xad); ////AD.Set IREF
	OLED_WR_Byte(0x90); ////Select:: Internal

	OLED_WR_REG(0xb3); ////B3.Set Display Clock Divide Ratio/Oscillator Frequency
	OLED_WR_Byte(0x61); ////DivClk:: 0~255

	OLED_WR_REG(0xb9); ////B9 Default GAMMA

	OLED_WR_REG(0xAF); //Set Display On

}




