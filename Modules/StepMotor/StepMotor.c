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

static StepMotorInstance *step_motor_instance[STEP_MOTOR_CNT] = {NULL};
static uint8_t step_motor_idx = 0; // register step_motor_idx,是该文件的全局舵机索引,在注册时使用

/**
 * @brief 电机控制函数
 * 
 */
void StepMotorControl(StepMotorInstance* motor)
{
    if (motor == NULL || motor->usart_instance == NULL) {
        // 检查输入参数是否有效
        return;
    }

    // 创建一个数据帧
    StepMotorDataFrame_s DataFrame = GenerateDataFrame(motor);

    // 发送数据帧
    USARTSend(motor->usart_instance, DataFrame, sizeof(DataFrame), USART_TRANSFER_DMA);}

// 串口回调函数实现
void MyUSARTCallback() 
{

#ifdef USARTCALLBACK

    //一个 USART 实例指针
    extern USARTInstance *global_usart_instance;
    
    // 获取接收到的数据
    USARTInstance *instance = global_usart_instance;
    if (instance == NULL) {
        return;
    }

    uint8_t *recv_buffer = instance->recv_buff;
    uint8_t recv_size = instance->recv_buff_size;

    // 检查接收数据的大小
    if (recv_size < sizeof(DataFrame)) {
        return;
    }

    // 解析数据帧
    DataFrame frame;
    memcpy(&frame, recv_buffer, sizeof(DataFrame));

    // 简单的帧头和帧尾检查
    if (frame.frame_header != 0x7B || frame.frame_footer != 0x7D) {
        // 帧头或帧尾错误
        return;
    }

    // 校验位检查
    uint8_t calculated_bcc = 0;
    for (int i = 0; i < sizeof(DataFrame) - 1; i++) {
        calculated_bcc ^= ((uint8_t *)&frame)[i];
    }
    if (frame.bcc != calculated_bcc) {
        // 校验位错误
        return;
    }

    // 处理数据帧
    switch (frame.control_mode) {
        case 0x01:
            // 速度控制模式
            // 处理速度控制逻辑
            break;
        case 0x02:
            // 位置控制模式
            // 处理位置控制逻辑
            break;
        case 0x03:
            // 力矩控制模式
            // 处理力矩控制逻辑
            break;
        case 0x04:
            // 单圈绝对角度控制模式
            // 处理绝对角度控制逻辑
            break;
        default:
            // 无效的控制模式
            return;
    }
#endif // USARTCALLBACK

    // 根据控制模式处理数据
    // 例如，可以使用 frame.subdivision、frame.data_high 和 frame.data_low 来设置步进电机参数

    // 其他处理逻辑
}
/**
 * @brief 步进电机实例注册函数
 * 
 * @param StepMotor_Init_Config_s 
 * @return StepMotorInstance* 
 */
StepMotorInstance *StepMotorRegister(StepMotor_Init_Config_s *StepMotor_Init_Config)
{
    StepMotorInstance *motor = (StepMotorInstance *)malloc(sizeof(StepMotorInstance));
    memset(motor, 0, sizeof(StepMotorInstance));

    //成员变量初始化
    motor->control_id = StepMotor_Init_Config->control_id;
    motor->control_mode = StepMotor_Init_Config->control_mode;
    motor->motor_direction = StepMotor_Init_Config->motor_direction;
    motor->subdivision = StepMotor_Init_Config->subdivision;
    motor->data = StepMotor_Init_Config->data;
    motor->speed = StepMotor_Init_Config->speed;
    motor->usart_handle = StepMotor_Init_Config->usart_handle;
    motor->usart_instance = StepMotor_Init_Config->usart_instance;

    //为这个步进电机实例化一个USART

    //串口初始化配置
    USART_Init_Config_s usart_config;
    usart_config.recv_buff_size = USART_RXBUFF_LIMIT; // 设置接收缓冲区大小
    usart_config.usart_handle = &huart1; // 使用具体的 UART_HandleTypeDef 实例
    usart_config.module_callback = MyUSARTCallback; // 设置解析数据的回调函数

    //注册串口实例
    USARTInstance *usart_instance = USARTRegister(&usart_config);
    if (usart_instance == NULL) {
        // 处理串口注册失败的情况
        return;
    }
    
    //将这个新步进电机加入步进电机序列
    step_motor_instance[step_motor_idx++] = motor;
    return motor;

}
