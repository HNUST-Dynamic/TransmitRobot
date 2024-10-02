#include "servoMotor.h"
#include "stdlib.h"
#include "memory.h"
#include "stm32f4xx_hal_tim.h"
#include "bsp_pwm.h"

extern TIM_HandleTypeDef htim1;

static ServoInstance* servo_motor_instance[SERVO_MOTOR_CNT] = {NULL} ;
static int16_t compare_value[SERVO_MOTOR_CNT] = {0};
static uint32_t pluse;
static  uint8_t servo_idx = 3;

//注册舵机
ServoInstance *ServoInit(Servo_Init_Config_s *Servo_Init_Config)
{
    ServoInstance *servo = (ServoInstance *)malloc(sizeof(ServoInstance));
    memset(servo, 0, sizeof(ServoInstance));

    servo->angle =Servo_Init_Config->angle;
    servo->htim = Servo_Init_Config->htim;
    servo->Channel = Servo_Init_Config->Channel;

    HAL_TIM_PWM_Start(Servo_Init_Config->htim, Servo_Init_Config->Channel);
    servo_motor_instance[servo_idx++] = servo;
    return servo;
}

//初始化舵机配置
//只是一个封装
void ServoMotor_Init()
{
    HAL_TIM_MspPostInit(&htim1);
}

//单次多个舵机控制
void ServoMotor_Control()
{
  ServoInstance* Servo_Motor;
  for (size_t i =0; i < servo_idx ; i++)
  {
    if(servo_motor_instance[i])
    {
        compare_value[i] = 0.5 * 20000 / 20 + Servo_Motor->angle * 20000 / 20 / 135;       
        __HAL_TIM_SET_COMPARE(&htim1,servo_motor_instance[i]->Channel, compare_value[i]);
    }
  }
   
}
//单次单个舵机控制
void ServoMotor_Set_Angle(ServoInstance* xServo,uint16_t xangle)
{
    xServo->angle =xangle;
    //uint32_t pulse_min = 1167;  // 对应0°的脉冲宽度 (0.5ms)
    //uint32_t pulse_max = 5833;  // 对应270°的脉冲宽度 (2.5ms)
    
    // 计算实际脉宽
    //uint32_t pulse = pulse_min + (xServo->angle * (pulse_max - pulse_min)) / 270;
    // uint32_t pluse =0.5 * 20000 / 20 + xServo->angle * 20000 / 20 / 135;
    __HAL_TIM_SET_COMPARE(&htim1, xServo->Channel,xServo->angle );

}
