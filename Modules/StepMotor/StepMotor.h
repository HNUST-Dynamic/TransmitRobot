#ifndef STEPMOTOR_H_
#define STEPMOTOR_H_

#include "bsp_usart.h"
#include "main.h"
#include "stdbool.h"

#define STEP_MOTOR_CNT 5
#define STEPS_PER_REVOLUTION 3200   // 假设电机每转一圈是200*16步（在未细分的情况下）
#define MICROSTEPS 256             // 电机的细分数

extern volatile uint16_t encoder_value;
/**
 * @brief 步进电机控制模式枚举
 * 
 */
typedef enum
{
    SpeedMode = 0x01,       //速度模式
    PosMode = 0x02,         //位置模式

} Step_Mode_e;

typedef enum
{  
    OffPulMode = (uint8_t)0,
    OpenCircuit = (uint8_t)1,
    CloseCircuit = (uint8_t)2,
    Multiplexed = (uint8_t)3,

} Ctrl_Mode_e;

typedef enum
{
    ClockWise = (bool)1,              //顺时针
    CounterClockWise = (bool)0,       //逆时针
} Motor_Direction_e;

/**
 * @brief 步进电机初始化设置结构体
 * 
 */
typedef struct 
{
    Step_Mode_e step_mode;    // 电机模式 速度 位置
    Ctrl_Mode_e ctrl_mode;     //控制模式
    Motor_Direction_e motor_direction; // 转向: 0=逆时针, 1=顺时针
    uint16_t speed;           // 速度,单位为 RPM
    uint8_t acc;              //加速度 0直接启动
    UART_HandleTypeDef* usart_handle; // 使用具体的 UART_HandleTypeDef 实例

} StepMotor_Init_Config_s;

/**
 * @brief 步进电机实例
 * 
 */
typedef struct 
{
    Step_Mode_e step_mode;    // 电机模式 速度 位置
    Ctrl_Mode_e ctrl_mode;     //控制模式
    Motor_Direction_e motor_direction; // 转向: 0=逆时针, 1=顺时针
    uint16_t speed;           // 速度,单位为 RPM
    uint8_t acc;              //加速度 0直接启动
    uint32_t clk;
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
 * @param s         系统参数类型
 */
void StepMotorReadParams(StepMotorInstance* motor, SysParams_e s);

/**
 * @brief 电机修改开环/闭环控制模式
 * 
 * @param motor 
 * @param svF           是否存储标志，false为不存储，true为存储
 */
void StepMotorModifyCtrlMode(StepMotorInstance* motor, bool svF);

/**
 * @brief 电机使能信号控制
 * 
 * @param motor 
 * @param state         使能状态 true为使能电机，false为关闭电机
 * @param snF           多机同步标志，false为不启用，true为启用
 */
void StepMotorEnControl(StepMotorInstance* motor, bool state,bool snF);

/**
 * @brief 电机速度模式设置
 * 
 * @param motor 
 * @param snF       多机同步标志 false true
 */
void StepMotorVelControl(StepMotorInstance* motor, bool snF);

/**
 * @brief 电机位置模式设置
 * 
 * @param motor 
 * @param clk       脉冲数 0-（2^32 - 1）
 * @param raF       相位/绝对标志 false为相对运动 ， true为绝对运动
 * @param snF       多机同步标志
 */
void StepMotorPosControl(StepMotorInstance* motor, bool raF, bool snF);

/**
 * @brief 电机立即停止
 * 
 * @param motor 
 * @param snF   多机同步标志
 */
void StepMotorStop(StepMotorInstance* motor, bool snF);

/**
 * @brief 电机多机同步运动
 * 
 * @param motor 
 */
void StepMotorSynMotion(StepMotorInstance* motor);

#endif // !STEPMOTOR_H_