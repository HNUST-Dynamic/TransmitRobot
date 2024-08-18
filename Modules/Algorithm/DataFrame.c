/**
 * @file DataFrame.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-18
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include "DataFrame.h"
#include "stdlib.h"

/**
 * @brief 生成数据帧
 * 
 * @param motor_instance 
 * @return StepMotorDataFrame_s 
 */
StepMotorDataFrame_s GenerateDataFrame(StepMotorInstance *motor_instance) 
{
    StepMotorDataFrame_s frame;

    // 设置固定的帧头和控制ID
    frame.frame_header = 0x7B;
    frame.control_id = 0x01;

    // 从 StepMotorInstance 中获取数据
    frame.control_mode = motor_instance->control_mode;
    frame.motor_direction = motor_instance->motor_direction;
    frame.subdivision = motor_instance->subdivision;

    // 将数据值和转速值转换为高低八位
    frame.data_high = (motor_instance->data * 10) >> 8;
    frame.data_low = (motor_instance->data * 10) & 0xFF;
    frame.speed_high = (motor_instance->speed * 10) >> 8;
    frame.speed_low = (motor_instance->speed * 10) & 0xFF;

    // 计算 BCC 校验位
    frame.rcc_checksum = CalculateRcc(&frame);

    // 设置固定的帧尾
    frame.frame_tail = 0x7D;

    return frame;
}

/**
 * @brief RCC校验位计算
 * 
 * @param frame 
 * @return uint8_t 
 */
uint8_t CalculateRcc(DataFrame *frame) 
{
    uint8_t rcc = 0;
    rcc ^= frame->frame_header;
    rcc ^= frame->control_id;
    rcc ^= frame->control_mode;
    rcc ^= frame->motor_direction;
    rcc ^= frame->subdivision;
    rcc ^= frame->data_high;
    rcc ^= frame->data_low;
    rcc ^= frame->speed_high;
    rcc ^= frame->speed_low;
    return rcc;
}