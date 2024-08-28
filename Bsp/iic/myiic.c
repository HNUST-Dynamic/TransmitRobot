/**
 ****************************************************************************************************
 * @file        myiic.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-04-20
 * @brief       IIC 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 阿波罗 F429开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20220420
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "myiic.h"

/**
 * @brief       初始化IIC
 * @param       无
 * @retval      无
 */
void iic_init(void)
{
    //Delay_Init();
    GPIO_InitTypeDef gpio_init_struct;

    IIC_SCL_GPIO_CLK_ENABLE();                              /* SCL引脚时钟使能 */
    IIC_SDA_GPIO_CLK_ENABLE();                              /* SDA引脚时钟使能 */

    gpio_init_struct.Pin = IIC_SCL_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;            /* 推挽输出 */
    gpio_init_struct.Pull = GPIO_PULLUP;                    /* 上拉 */
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;     /* 快速 */
    HAL_GPIO_Init(IIC_SCL_GPIO_PORT, &gpio_init_struct);    /* SCL引脚初始化 */

    /* SDA引脚开漏输出,上拉, 这样就不用再设置IO方向了,开漏输出的时候(=1), 也可以读取外部信号的高低电平 */
    gpio_init_struct.Pin = IIC_SDA_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_OD;            /* 开漏输出 */
    HAL_GPIO_Init(IIC_SDA_GPIO_PORT, &gpio_init_struct);    /* SDA引脚初始化 */

    iic_stop();                                             /* 停止总线上所有设备 */
}

/**
 * @brief       IIC延时函数,用于控制IIC读写速度
 * @param       无
 * @retval      无
 */
static void iic_delay(void)
{
    Delay_us(2);            /* 2us的延时, 读写速度在250Khz以内 */
}
void Delay(uint32_t count)//400KHzIIC
{
	unsigned int uiCnt = count*8;
	while (uiCnt --);
}

/**
 * @brief       产生IIC起始信号
 * @param       无
 * @retval      无
 */
void iic_start(void)
{
    IIC_SDA(1);
    IIC_SCL(1);
    Delay(5);
    IIC_SDA(0);             /* START信号: 当SCL为高时, SDA从高变成低, 表示起始信号 */
    Delay(5);
    IIC_SCL(0);             /* 钳住I2C总线，准备发送或接收数据 */
    Delay(5);
}

/**
 * @brief       产生IIC停止信号
 * @param       无
 * @retval      无
 */
void iic_stop(void)
{
    IIC_SDA(0);             /* STOP信号: 当SCL为高时, SDA从低变成高, 表示停止信号 */
    Delay(5);
    IIC_SCL(1);
    Delay(5);
    IIC_SDA(1);             /* 发送I2C总线结束信号 */
    Delay(5);
}

/**
 * @brief       等待应答信号到来
 * @param       无
 * @retval      1，接收应答失败
 *              0，接收应答成功
 */
uint8_t iic_wait_ack(void)
{
    uint8_t waittime = 0;
    uint8_t rack = 0;

    IIC_SDA(1);             /* 主机释放SDA线(此时外部器件可以拉低SDA线) */
    Delay(5);
    IIC_SCL(1);             /* SCL=1, 此时从机可以返回ACK */
    Delay(5);

    while (IIC_READ_SDA)    /* 等待应答 */
    {
        waittime++;

        if (waittime > 250)
        {
            iic_stop();
            rack = 1;
            break;
        }
    }

    IIC_SCL(0);             /* SCL=0, 结束ACK检查 */
    Delay(5);
    return rack;
}

/**
 * @brief       产生ACK应答
 * @param       无
 * @retval      无
 */
void iic_ack(void)
{
    IIC_SDA(0);             /* SCL 0 -> 1 时 SDA = 0,表示应答 */
    Delay(5);
    IIC_SCL(1);             /* 产生一个时钟 */
    Delay(5);
    IIC_SCL(0);
    Delay(5);
    IIC_SDA(1);             /* 主机释放SDA线 */
    Delay(5);
}

/**
 * @brief       不产生ACK应答
 * @param       无
 * @retval      无
 */
void iic_nack(void)
{
    IIC_SDA(1);             /* SCL 0 -> 1  时 SDA = 1,表示不应答 */
    Delay(5);
    IIC_SCL(1);             /* 产生一个时钟 */
    Delay(5);
    IIC_SCL(0);
    Delay(5);
}

/**
 * @brief       IIC发送一个字节
 * @param       data: 要发送的数据
 * @retval      无
 */
void iic_send_byte(uint8_t data)
{
    uint8_t t;
    
    for (t = 0; t < 8; t++)
    {
        IIC_SDA((data & 0x80) >> 7);    /* 高位先发送 */
        Delay(5);
        IIC_SCL(1);
        Delay(5);
        IIC_SCL(0);
        data <<= 1;             /* 左移1位,用于下一次发送 */
    }
    IIC_SDA(1);                 /* 发送完成, 主机释放SDA线 */
}

/**
 * @brief       IIC读取一个字节
 * @param       ack:  ack=1时，发送ack; ack=0时，发送nack
 * @retval      接收到的数据
 */
uint8_t iic_read_byte(uint8_t ack)
{
    uint8_t i, receive = 0;

    for (i = 0; i < 8; i++ )    /* 接收1个字节数据 */
    {
        receive <<= 1;          /* 高位先输出,所以先收到的数据位要左移 */
        IIC_SCL(1);
        Delay(5);

        if (IIC_READ_SDA)
        {
            receive++;
        }
        
        IIC_SCL(0);
        Delay(5);
    }

    if (!ack)
    {
        iic_nack();             /* 发送nACK */
    }
    else
    {
        iic_ack();              /* 发送ACK */
    }

    return receive;
}








