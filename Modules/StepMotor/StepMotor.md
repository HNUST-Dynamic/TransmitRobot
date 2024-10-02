# StepMotor Module

## 注册一个步进电机的流程
1. 实例化一个 StepMotor_Init_Config_s 结构体和一个 StepMotorInstance 实例
2. 给StepMotor_Init_Config_s设定参数，指定电机控制函数
3. 调用StepMotorInit函数，给它传入 StepMotor_Init_Config_s 结构体 ，接收它返回的StepMotorInstance实例

## 控制一个电机的方法
调用已经注册的StepMotorInstance 实例中的control方法