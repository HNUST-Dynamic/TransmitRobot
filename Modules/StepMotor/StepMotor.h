#ifndef STEPMOTOR_H_
#define STEPMOTOR_H_

#include "bsp_usart.h"
#include "main.h"
#include "stdbool.h"

#define STEP_MOTOR_CNT 5

// 定义一个函数指针类型，用于电机控制
typedef void (*MotorControlFunction)(struct StepMotorInstance* motor);

/**
 * @brief 步进电机控制模式枚举
 * 
 */
typedef enum
{
    SpeedMode = 0x01,       //速度模式
    PosMode = 0x02,         //位置模式
    ForceMode = 0x03,       //力矩模式
} Control_Mode_e;

typedef enum
{
    ClockWise,              //顺时针
    CounterClockWise,       //逆时针
} Motor_Direction_e;

/**
 * @brief 步进电机初始化设置结构体
 * 
 */
typedef struct 
{
    uint8_t control_id;      // 控制ID: 默认为0x01
    Control_Mode_e control_mode;    // 控制模式: 0x01=速度控制, 0x02=位置控制, 0x03=力矩控制, 0x04=单圈绝对角度控制
    Motor_Direction_e motor_direction; // 转向: 0=逆时针, 1=顺时针
    uint8_t subdivision;     // 细分值: 例如0x20=32细分
    uint16_t data;            // 速度控制模式下为 0；
                             // 位置控制模式下的角度数据，单位为角度;
                             // 力矩控制模式下的电流大小数据，单位为毫安
    uint16_t speed;           // 速度,单位为 rad/s
    UART_HandleTypeDef* usart_handle; // 使用具体的 UART_HandleTypeDef 实例

} StepMotor_Init_Config_s;

/**
 * @brief 步进电机实例
 * 
 */
typedef struct 
{
    uint8_t control_id;      // 控制ID: 默认为0x01
    Control_Mode_e control_mode;    // 控制模式: 0x01=速度控制, 0x02=位置控制, 0x03=力矩控制, 0x04=单圈绝对角度控制
    Motor_Direction_e motor_direction; // 转向: 0=逆时针, 1=顺时针
    uint8_t subdivision;     // 细分值: 例如0x20=32细分
    uint16_t data;            // 速度控制模式下为 0；
                             // 位置控制模式下的角度数据，单位为角度;
                             // 力矩控制模式下的电流大小数据，单位为毫安
    uint16_t speed;           // 速度,单位为 rad/s
    UART_HandleTypeDef* usart_handle; // 使用具体的 UART_HandleTypeDef 实例
    USARTInstance* usart_instance; // 添加指向 USART 实例的指针

} StepMotorInstance;

typedef enum {
	S_VER   = 0,			/* 读取固件版本和对应的硬件版本 */
	S_RL    = 1,			/* 读取读取相电阻和相电感 */
	S_PID   = 2,			/* 读取PID参数 */
	S_VBUS  = 3,			/* 读取总线电压 */
	S_CPHA  = 5,			/* 读取相电流 */
	S_ENCL  = 7,			/* 读取经过线性化校准后的编码器值 */
	S_TPOS  = 8,			/* 读取电机目标位置角度 */
	S_VEL   = 9,			/* 读取电机实时转速 */
	S_CPOS  = 10,			/* 读取电机实时位置角度 */
	S_PERR  = 11,			/* 读取电机位置误差角度 */
	S_FLAG  = 13,			/* 读取使能/到位/堵转状态标志位 */
	S_Conf  = 14,			/* 读取驱动参数 */
	S_State = 15,			/* 读取系统状态参数 */
	S_ORG   = 16,     /* 读取正在回零/回零失败状态标志位 */
}SysParams_e;

/**
 * @brief 电机注册函数
 * 
 * @param StepMotor_Init_Config 
 * @return StepMotorInstance* 
 */
StepMotorInstance *StepMotorRegister(StepMotor_Init_Config_s *StepMotor_Init_Config);

/**
 * @brief 电机位置清零
 * 
 * @param motor 
 */
void StepMotorResetZero(StepMotorInstance* motor);

/**
 * @brief 电机解除堵转保护
 * 
 * @param motor 
 */
void StepMotorResetClogPro(StepMotorInstance* motor);

/**
 * @brief 电机读取参数
 * 
 * @param motor 
 * @param s 
 */
void StepMotorReadParams(StepMotorInstance* motor, SysParams_e s);

/**
 * @brief 电机修改开环/闭环控制模式
 * 
 * @param motor 
 * @param svF 
 * @param ctrl_mode 
 */
void StepMotorModifyCtrlMode(StepMotorInstance* motor, bool svF, uint8_t ctrl_mode);

/**
 * @brief 电机使能信号控制
 * 
 * @param motor 
 * @param state 
 * @param snF 
 */
void StepMotorEnControl(StepMotorInstance* motor, bool state, bool snF);

/**
 * @brief 电机速度模式设置
 * 
 * @param motor 
 * @param dir 
 * @param vel 
 * @param acc 
 * @param snF 
 */
void StepMotorVelControl(StepMotorInstance* motor, uint8_t dir, uint16_t vel, uint8_t acc, bool snF);

/**
 * @brief 电机位置模式设置
 * 
 * @param motor 
 * @param dir 
 * @param vel 
 * @param acc 
 * @param clk 
 * @param raF 
 * @param snF 
 */
void StepMotorPosControl(StepMotorInstance* motor, uint8_t dir, uint16_t vel, uint8_t acc, uint32_t clk, bool raF, bool snF);

/**
 * @brief 电机立即停止
 * 
 * @param motor 
 * @param snF 
 */
void StepMotorStop(StepMotorInstance* motor, bool snF);

/**
 * @brief 电机多机同步运动
 * 
 * @param motor 
 */
void StepMotorSynMotion(StepMotorInstance* motor);

#endif // !STEPMOTOR_H_