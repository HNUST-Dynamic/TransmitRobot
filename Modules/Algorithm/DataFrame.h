#ifndef DATAFRAME_H_
#define DATAFRAME_H_

#include <stdint.h>
#include "main.h"
#include "StepMotor.h"

typedef struct {
    uint8_t frame_header;    // 帧头: 0x7B
    uint8_t control_id;      // 控制ID: 默认为0x01
    uint8_t control_mode;    // 控制模式: 0x01=速度控制, 0x02=位置控制, 0x03=力矩控制, 0x04=单圈绝对角度控制
    uint8_t motor_direction; // 转向: 0=逆时针, 1=顺时针
    uint8_t subdivision;     // 细分值: 例如0x20=32细分
    uint8_t data_high;       // 数据高八位
    uint8_t data_low;        // 数据低八位
    uint8_t speed_high;      // 转速高八位
    uint8_t speed_low;       // 转速低八位
    uint8_t rcc_checksum;    // 校验位: 前9个字节的异或和
    uint8_t frame_tail;      // 帧尾: 0x7D
} StepMotorDataFrame_s;
StepMotorDataFrame_s GenerateDataFrame(StepMotorInstance *motor_instance); 

#endif // !DATAFRAME_H_
