/**
 * @file StepMotor.c
 * @author noneofever (noneofever@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "StepMotor.h"
#include "bsp_usart.h"
#include "memory.h"
#include "usart.h"

#define USARTCALLBACK

static StepMotorInstance *step_motor_instance[STEP_MOTOR_CNT] = {NULL};
static uint8_t step_motor_idx = 0; 
// 串口回调函数实现
void MyUSARTCallback() 
{

#ifdef USARTCALLBACK
	__IO uint16_t i = 0;

/**********************************************************
***	串口接收中断
**********************************************************/
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		// 未完成一帧数据接收，数据进入缓冲队列
		fifo_enQueue((uint8_t)USART1->DR);

		// 清除串口接收中断
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}

/**********************************************************
***	串口空闲中断
**********************************************************/
	else if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		// 先读SR再读DR，清除IDLE中断
		USART1->SR; USART1->DR;

		// 提取一帧数据命令
		rxCount = fifo_queueLength(); for(i=0; i < rxCount; i++) { rxCmd[i] = fifo_deQueue(); }

		// 一帧数据接收完成，置位帧标志位
		rxFrameFlag = true;
	}


#endif // USARTCALLBACK

    // 根据控制模式处理数据
    // 例如，可以使用 frame.subdivision、frame.data_high 和 frame.data_low 来设置步进电机参数

    // 其他处理逻辑
}

StepMotorInstance *StepMotorRegister(StepMotor_Init_Config_s *StepMotor_Init_Config)
{
    StepMotorInstance *motor = (StepMotorInstance *)malloc(sizeof(StepMotorInstance));
    memset(motor, 0, sizeof(StepMotorInstance));

    //成员变量初始化
    motor->step_mode = StepMotor_Init_Config->step_mode;
    motor->ctrl_mode = StepMotor_Init_Config->ctrl_mode;
    motor->motor_direction = StepMotor_Init_Config->motor_direction;
    motor->speed = StepMotor_Init_Config->speed;
    motor->usart_handle = StepMotor_Init_Config->usart_handle;

    //为这个步进电机实例化一个USART

    //串口初始化配置
    USART_Init_Config_s usart_config;
    usart_config.recv_buff_size = USART_RXBUFF_LIMIT; // 设置接收缓冲区大小
    usart_config.usart_handle = motor->usart_handle; // 使用具体的 UART_HandleTypeDef 实例
    usart_config.module_callback = MyUSARTCallback; // 设置解析数据的回调函数

    //注册串口实例
    motor->usart_instance = USARTRegister(&usart_config);
    if ( motor->usart_instance == NULL)
    {
        // 处理串口注册失败的情况
        return;
    }
    
    //将这个新步进电机加入步进电机序列
    step_motor_instance[step_motor_idx++] = motor;
    return motor;

}

void StepMotorResetZero(StepMotorInstance* motor)
{
    static uint8_t cmd[16] = {0};
    
    // 装载命令
    cmd[0] =  0x01;                       // 地址
    cmd[1] =  0x0A;                       // 功能码
    cmd[2] =  0x6D;                       // 辅助码
    cmd[3] =  0x6B;                       // 校验字节
    
    // 发送命令
    USARTSend(motor->usart_instance,cmd, 4,USART_TRANSFER_DMA);
}

void StepMotorResetClogPro(StepMotorInstance* motor)
{
    static uint8_t cmd[16] = {0};
    
    // 装载命令
    cmd[0] =  0x01;                       // 地址
    cmd[1] =  0x0E;                       // 功能码
    cmd[2] =  0x52;                       // 辅助码
    cmd[3] =  0x6B;                       // 校验字节
    
    // 发送命令
    USARTSend(motor->usart_instance,cmd, 4,USART_TRANSFER_DMA);
}

void StepMotorReadParams(StepMotorInstance* motor, SysParams_e s)
{
    uint8_t i = 0;
    static uint8_t cmd[16] = {0};
    
    // 装载命令
    cmd[i] = 0x01; ++i;                   // 地址

    switch(s)                             // 功能码
    {
        case S_VER  : cmd[i] = 0x1F; ++i; break;
        case S_RL   : cmd[i] = 0x20; ++i; break;
        case S_PID  : cmd[i] = 0x21; ++i; break;
        case S_VBUS : cmd[i] = 0x24; ++i; break;
        case S_CPHA : cmd[i] = 0x27; ++i; break;
        case S_ENCL : cmd[i] = 0x31; ++i; break;
        case S_TPOS : cmd[i] = 0x33; ++i; break;
        case S_VEL  : cmd[i] = 0x35; ++i; break;
        case S_CPOS : cmd[i] = 0x36; ++i; break;
        case S_PERR : cmd[i] = 0x37; ++i; break;
        case S_FLAG : cmd[i] = 0x3A; ++i; break;
        case S_ORG  : cmd[i] = 0x3B; ++i; break;
        case S_Conf : cmd[i] = 0x42; ++i; cmd[i] = 0x6C; ++i; break;
        case S_State: cmd[i] = 0x43; ++i; cmd[i] = 0x7A; ++i; break;
        default: break;
    }

    cmd[i] = 0x6B; ++i;                   // 校验字节
    
    // 发送命令
    USARTSend(motor->usart_instance,cmd, sizeof(cmd),USART_TRANSFER_DMA);
}

void StepMotorModifyCtrlMode(StepMotorInstance* motor, bool svF)
{
  static uint8_t cmd[16] = {0};
  
  // 装载命令
  cmd[0] =  0x01;                       // 地址
  cmd[1] =  0x46;                       // 功能码
  cmd[2] =  0x69;                       // 辅助码
  cmd[3] =  svF;                        // 是否存储标志，false为不存储，true为存储
  cmd[4] =  motor->ctrl_mode;                  // 控制模式（对应屏幕上的P_Pul菜单），0是关闭脉冲输入引脚，1是开环模式，2是闭环模式，3是让En端口复用为多圈限位开关输入引脚，Dir端口复用为到位输出高电平功能
  cmd[5] =  0x6B;                       // 校验字节
  
  // 发送命令
    USARTSend(motor->usart_instance,cmd, 6,USART_TRANSFER_DMA);
}

void StepMotorEnControl(StepMotorInstance* motor, bool state, bool snF)
{
    static uint8_t cmd[16] = {0};
    
    // 装载命令
    cmd[0] =  0x01;                       // 地址
    cmd[1] =  0xF3;                       // 功能码
    cmd[2] =  0xAB;                       // 辅助码
    cmd[3] =  (uint8_t)state;             // 使能状态
    cmd[4] =  snF;                        // 多机同步运动标志
    cmd[5] =  0x6B;                       // 校验字节
    
    // 发送命令
    USARTSend(motor->usart_instance,cmd, 6,USART_TRANSFER_DMA);
}

void StepMotorVelControl(StepMotorInstance* motor, bool snF)
{
    static uint8_t cmd[16] = {0};

    // 装载命令
    cmd[0] =  0x01;                       // 地址
    cmd[1] =  0xF6;                       // 功能码
    cmd[2] =  motor->motor_direction;                        // 方向
    cmd[3] =  (uint8_t)(motor->speed >> 8);        // 速度(RPM)高8位字节
    cmd[4] =  (uint8_t)(motor->speed >> 0);        // 速度(RPM)低8位字节
    cmd[5] =  motor->acc;                        // 加速度，注意：0是直接启动
    cmd[6] =  snF;                        // 多机同步运动标志
    cmd[7] =  0x6B;                       // 校验字节
    
    // 发送命令
    USARTSend(motor->usart_instance,cmd, 8,USART_TRANSFER_DMA);
}

void StepMotorPosControl(StepMotorInstance* motor, bool raF, bool snF)
{
    static uint8_t cmd[16] = {0};

    // 装载命令
    cmd[0]  =  0x01;                      // 地址
    cmd[1]  =  0xFD;                      // 功能码
    cmd[2]  =  motor->motor_direction;                       // 方向
    cmd[3]  =  (uint8_t)(motor->speed >> 8);       // 速度(RPM)高8位字节
    cmd[4]  =  (uint8_t)(motor->speed >> 0);       // 速度(RPM)低8位字节 
    cmd[5]  =  motor->acc;                       // 加速度，注意：0是直接启动
    cmd[6]  =  (uint8_t)(motor->clk >> 24);      // 脉冲数(bit24 - bit31)
    cmd[7]  =  (uint8_t)(motor->clk >> 16);      // 脉冲数(bit16 - bit23)
    cmd[8]  =  (uint8_t)(motor->clk >> 8);       // 脉冲数(bit8  - bit15)
    cmd[9]  =  (uint8_t)(motor->clk >> 0);       // 脉冲数(bit0  - bit7 )
    cmd[10] =  raF;                       // 相位/绝对标志，false为相对运动，true为绝对值运动
    cmd[11] =  snF;                       // 多机同步运动标志，false为不启用，true为启用
    cmd[12] =  0x6B;                      // 校验字节
    
    // 发送命令
    USARTSend(motor->usart_instance,cmd, 13,USART_TRANSFER_DMA);

}

void StepMotorStop(StepMotorInstance* motor, bool snF)
{
    static uint8_t cmd[16] = {0};
    
    // 装载命令
    cmd[0] =  0x01;                       // 地址
    cmd[1] =  0xFE;                       // 功能码
    cmd[2] =  0x98;                       // 辅助码
    cmd[3] =  snF;                        // 多机同步运动标志
    cmd[4] =  0x6B;                       // 校验字节
    
    // 发送命令
    USARTSend(motor->usart_instance,cmd, 5,USART_TRANSFER_DMA);

}

void StepMotorSynMotion(StepMotorInstance* motor)
{
    static uint8_t cmd[16] = {0};
    
    // 装载命令
    cmd[0] =  0x01;                       // 地址
    cmd[1] =  0xFF;                       // 功能码
    cmd[2] =  0x66;                       // 辅助码
    cmd[3] =  0x6B;                       // 校验字节
    
    // 发送命令
    USARTSend(motor->usart_instance,cmd, 4,USART_TRANSFER_DMA);
}


