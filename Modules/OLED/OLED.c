#include "OLED.h"
#include "usart.h"
#include "bsp_usart.h"
#include "memory.h"
#include "oledfont.h"
#include "bmp.h"
#include "stm32f4xx_hal.h"
#include "i2c.h"
#include "stdlib.h"


// OLED_Fill(0,0,256,128,0x00);
// OLED_ShowString(44,90,"PIXELS:256x128",24,0);

/////////////////////////////

//延时
void IIC_delay(void)
{
	u8 t=3;
	while(t--);
}

//起始信号
void I2C_Start(void)
{
	OLED_SDA_Set();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SDA_Clr();
	IIC_delay();
	OLED_SCL_Clr();
	 
}

//结束信号
void I2C_Stop(void)
{
	OLED_SDA_Clr();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SDA_Set();
}

//等待信号响应
void I2C_WaitAck(void) //测数据信号的电平
{
	OLED_SDA_Set();
	OLED_SCL_Set();
	IIC_delay();
	OLED_SCL_Clr();
}

//写入一个字节
void Send_Byte(u8 dat)
{
	u8 i;
	for(i=0;i<8;i++)
	{
		OLED_SCL_Clr();//将时钟信号设置为低电平
		if(dat&0x80)//将dat的8位从最高位依次写入
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
//    函数说明：OLED写入一个指令
//    入口数据：reg 指令
//    返回值：  无
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
//    函数说明：OLED写入一个数据
//    入口数据：dat 数据
//    返回值：  无
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
//    函数说明：OLED显示列的起始终止地址
//    入口数据：a  列的起始地址
//              b  列的终止地址
//    返回值：  无
//******************************************************************************
void Column_Address(u8 a,u8 b)
{
	OLED_WR_REG(0x15);       // Set Column Address
	OLED_WR_Byte(a+0x08);
	OLED_WR_Byte(b+0x08);
}


//******************************************************************************
//    函数说明：OLED显示行的起始终止地址
//    入口数据：a  行的起始地址
//              b  行的终止地址
//    返回值：  无
//******************************************************************************
void Row_Address(u8 a,u8 b)
{
	OLED_WR_REG(0x75);       // Row Column Address
	OLED_WR_Byte(a);
	OLED_WR_Byte(b);
	OLED_WR_REG(0x5C);    //写RAM命令
}

void OLED_Fill(u16 xstr,u16 ystr,u16 xend,u16 yend,u8 color)
{
	u8 x,y;
	xstr/=4; //column address 控制4列 
	xend/=4;
	Column_Address(xstr,xend-1);
	Row_Address(ystr,yend-1);
	for(x=xstr;x<xend;x++)
	{
		for(y=ystr;y<yend;y++)
		{
			OLED_WR_Byte(color); //控制4列 同时支持16阶灰度 所以需要2个字节
			OLED_WR_Byte(color);
    }
  }
}

//******************************************************************************
//    函数说明：OLED显示汉字
//    入口数据：x,y :起点坐标
//              *s  :要显示的汉字串
//              sizey 字符高度 
//              mode  0:正常显示；1：反色显示
//    返回值：  无
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
//    函数说明：OLED显示汉字
//    入口数据：x,y :起点坐标
//              *s  :要显示的汉字
//              sizey 字符高度 
//              mode  0:正常显示；1：反色显示
//    返回值：  无
//******************************************************************************

void OLED_ShowChinese16x16(u8 x,u8 y,u8 *s,u8 sizey,u8 mode)
{
	u8 i,j,k,DATA1=0,DATA2=0;
	u16 HZnum;//汉字数目
	u16 TypefaceNum;//一个字符所占字节大小
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;//字符所占字节数
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//统计汉字库数目
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
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
} 

//******************************************************************************
//    函数说明：OLED显示汉字
//    入口数据：x,y :起点坐标
//              *s  :要显示的汉字
//              sizey 字符高度 
//              mode  0:正常显示；1：反色显示
//    返回值：  无
//******************************************************************************

void OLED_ShowChinese24x24(u8 x,u8 y,u8 *s,u8 sizey,u8 mode)
{
	u8 i,j,k,DATA1=0,DATA2=0;
	u16 HZnum;//汉字数目
	u16 TypefaceNum;//一个字符所占字节大小
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;//字符所占字节数
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//统计汉字库数目
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
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
} 


//******************************************************************************
//    函数说明：OLED显示32x32汉字
//    入口数据：x,y :起点坐标
//              *s  :要显示的汉字
//              sizey 字符高度 
//              mode  0:正常显示；1：反色显示
//    返回值：  无
//******************************************************************************

void OLED_ShowChinese32x32(u8 x,u8 y,u8 *s,u8 sizey,u8 mode)
{
	u8 i,j,k,DATA1=0,DATA2=0;
	u16 HZnum;//汉字数目
	u16 TypefaceNum;//一个字符所占字节大小
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;//字符所占字节数
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//统计汉字库数目
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
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
} 



//******************************************************************************
//    函数说明：OLED显示字符函数 
//    此函数适用范围：字符宽度是4的倍数  字符高度是宽度的2倍
//    入口数据：x,y   起始坐标
//              chr   要写入的字符
//              sizey 字符高度 
//              mode  0:正常显示；1：反色显示
//    返回值：  无
//******************************************************************************

void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 sizey,u8 mode)
{
	u8 sizex,c,temp,t=2,DATA1=0,DATA2=0,m;
	u16 i,k,size2;
	x/=4;
	sizex=sizey/4/2; //除4是因为一个列地址线控制4列 除2是字宽:字高 1:2
	size2=(sizey/16+((sizey%16)?1:0))*sizey; //计算一个字符所占字节数
	c=chr-' '; //ASCII码值对应偏移后的值
	Column_Address(x,x+sizex-1);//设置列地址
	Row_Address(y,y+sizey-1);//设置行地址
	for(i=0;i<size2;i++)
	{
		if(sizey==16)
		{
			temp=ascii_1608[c][i]; //获取取模数据 
		}
		else if(sizey==24)
		{
			temp=ascii_2412[c][i];//12x24 ASCII码
		}
		else if(sizey==32)
		{
			temp=ascii_3216[c][i];//16x32 ASCII码
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
//    函数说明：OLED显示字符串
//    入口数据：x,y  起始坐标
//              *dp   要写入的字符
//              sizey 字符高度 
//              mode  0:正常显示；1：反色显示
//    返回值：  无
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
//    函数说明：m^n
//    入口数据：m:底数 n:指数
//    返回值：  result
//******************************************************************************
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)result*=m;    
	return result;
}


//******************************************************************************
//    函数说明：OLED显示变量
//    入口数据：x,y :起点坐标	 
//              num :要显示的变量
//              len :数字的位数
//              sizey 字符高度 
//              mode  0:正常显示；1：反色显示
//    返回值：  无
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
//    函数说明：显示灰度图片
//    入口数据：x,y :起点坐标
//              length 图片长度
//              width  图片宽度
//              BMP[] :要显示图片
//              mode  0:正常显示；1：反色显示
//    返回值：  无
//******************************************************************************
void OLED_DrawBMP(u8 x,u8 y,u16 length,u8 width,const u8 BMP[],u8 mode)
{
  u16 i,num;
	length=(length/4+((length%4)?1:0))*4;
	num=length*width/2/2; //计算一个灰度图片所占字节数 一次写入2个字节 所以再除以2
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
//    函数说明：显示单色图片
//    入口数据：x,y :起点坐标
//              length 图片长度 
//              width  图片宽度
//              BMP[] :要显示图片
//              mode  0:正常显示；1：反色显示
//    返回值：  无
//******************************************************************************
void OLED_DrawSingleBMP(u8 x,u8 y,u16 length,u8 width,const u8 BMP[],u8 mode)
{
	u8 t=2,DATA1=0,DATA2=0;
	u16 i,k,num=0;
	length=(length/8+((length%8)?1:0))*8;
	num=length/8*width;  //计算图片所占字节数
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

//OLED的初始化
void OLED_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pins : PA4 PA5 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5|GPIO_PIN_7, GPIO_PIN_SET);

  /*Configure GPIO pin : PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
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




