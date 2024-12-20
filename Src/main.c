/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ServoMotor.h"
#include "Lift.h"
#include "IMU.h"
#include "Chassis.h"
#include "Vision.h"
#include "bsp_usart.h"
// stepmotor.h
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/*-----------------功能模块开关------------------ */
/**
 * @note 如需开启跑图模式则需要关闭所有开关
 */
#define QR_CODE_ADAPT   0     // 二维码测距功能开关
#define RAW_ADAPT       1     // 原料区微调功能开关
#define RING_ADAPT      1     // 圆环区微调功能开关
#define LIFT            1     // Lift应用的开关 

/*----------------快捷调参宏定义-----------------*/
#define REDZONE 18   // 红色圆环距离
#define GREENZONE 0  // 绿色圆环距离
#define BLUEZONE -18 // 蓝色圆环距离
#define TURN_K 0.8   // 转向摩擦系数
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int zone[7];
int x, y;
uint16_t watch_dog = 0; // 微调程序中的看门狗
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/**
 * @brief 陀螺仪纠偏
 * 
 * @param TargetAngle 目标角度
 */
void CorrectError(float TargetAngle)
{
  IMURecive();
  float error, Last_Yaw_t = 0;
  error = TargetAngle - Yaw_t;
  if (error >= 0.5 || error <= -0.5)
  {
    if (error > 0)
    {
      ChassisRotate(CounterClockWise_Chassis, 10, TURN_K * error);
      HAL_Delay(1000);
    }
    else
    {
      ChassisRotate(ClockWise_Chassis, 10, -TURN_K * error);
      HAL_Delay(1000);
    }
    Yaw_t = 0;
  }
  else
  {
    return 0;
    Yaw_t = 0;
  }
}

/**
 * @brief 色环微调
 * 
 * @param current_ring 色环代码
 */
void MicroAdapt(char current_ring,uint16_t num)
{
  while (1)
  {
    watch_dog = 0;
    // memset(input_copy, 0, sizeof(input_copy));
    while (!Ring_IsStable(current_ring))
    {
    }
    if (line[0] == current_ring)
    {
      while (!((abs(330 - x_int) <= 3) && (abs(235 - y_int) <= 3))) // 320 250
      {
        if (330 - x_int < 0)
        {
          //ChassisTransiation(Back, 45, (uint32_t)(0.07 * (x_int - 330 + 0.5f)));
          if(x_int - 330 > 100){
            ChassisTransiation_Adapt(Back,10,256*20);
          }else if((x_int - 330 <= 100) && (x_int - 330 > 50)){
            ChassisTransiation_Adapt(Back,10,256*8);
          }else if((x_int - 330 <= 50) && (x_int - 330 > 10)){
            ChassisTransiation_Adapt(Back,10,256*4);
          }else if((x_int - 330 <= 10) && (x_int - 330 > 5)){
            ChassisTransiation_Adapt(Back,10,256*1);
          }
        }
        else if (330 - x_int >= 0)
        {
          if(330 - x_int > 100){
            ChassisTransiation_Adapt(Forward,10,256*20);
          }else if((330 - x_int <= 100) && (330 - x_int > 50)){
            ChassisTransiation_Adapt(Forward,10,256*8);
          }else if((330 - x_int <= 50) && (330 - x_int > 10)){
            ChassisTransiation_Adapt(Forward,10,256*4);
          }else if((330 - x_int <= 10) && (330 - x_int > 5)){
            ChassisTransiation_Adapt(Forward,10,256*1);
          }
        }
        if (235 - y_int < 0)
        {
          if(y_int - 235 > 100){
            ChassisTransiation_Adapt(Left,10,256*20);
          }else if((y_int - 235 <= 100) && (y_int - 235 > 50)){
            ChassisTransiation_Adapt(Left,10,256*8);
          }else if((y_int - 235 <= 50) && (y_int - 235 > 15)){
            ChassisTransiation_Adapt(Left,10,256*4);
          }else if((y_int - 235 <= 10) && (y_int - 235 > 5)){
            ChassisTransiation_Adapt(Left,10,256*1);
          }
        }
        else if (235 - y_int >= 0)
        {
          if(235 - y_int > 100){
            ChassisTransiation_Adapt(Right,10,256*30);
          }else if((235 - y_int <= 100) && (235 - y_int > 50)){
            ChassisTransiation_Adapt(Right,10,256*8);
          }else if((235 - y_int <= 50) && (235 - y_int > 15)){
            ChassisTransiation_Adapt(Right,10,256*4);
          }else if((235 - y_int <= 10) && (235 - y_int > 5)){
            ChassisTransiation_Adapt(Right,10,256*1);
          }
        }
        watch_dog++;
        if (watch_dog >= num)
        {
          break;
        }
          // memset(input_copy, 0, sizeof(input_copy));

        while (!Ring_IsStable(current_ring))
        {
        }
      }
      break;
    }
  }
}

/**
 * @brief 物料微调
 * 
 * @param current_goods 物料代码
 */
void MicroAdapt_Goods(char current_goods,uint16_t num,uint32_t steps)
{
  while (1)
  {
    watch_dog = 0;
    // memset(input_copy, 0, sizeof(input_copy));
    while (!IsStable(current_goods))
    {
    }
    if (line[0] == current_goods)
    {
      while (!((abs(330 - x_int) <= 5) && (abs(230 - y_int) <= 5))) // 320 250
      {
        if (330 - x_int < 0)
        {
          //ChassisTransiation(Back, 45, (uint32_t)(0.07 * (x_int - 330 + 0.5f)));
          ChassisTransiation_Adapt(Back,45,256*steps);
        }
        else if (330 - x_int >= 0)
        {
          //ChassisTransiation(Forward, 45, (uint32_t)(0.07 * (330 - x_int + 0.5f)));
          ChassisTransiation_Adapt(Forward,45,256*steps);
        }
        if (230 - y_int < 0)
        {
          //ChassisTransiation(Left, 45, (uint32_t)(0.07 * (y_int - 235 + 0.5f)));
          ChassisTransiation_Adapt(Left,45,256*steps);
        }
        else if (230 - y_int >= 0)
        {
          //ChassisTransiation(Right, 45, (uint32_t)(0.07 * (235 - y_int + 0.5f)));
          ChassisTransiation_Adapt(Right,45,256*steps);
        }
        watch_dog++;
        if (watch_dog >= num)
        {
          break;
        }
          // memset(input_copy, 0, sizeof(input_copy));

        while (!IsStable(current_goods))
        {
        }
      }
      break;
    }
  }
}

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint8_t timeout_count = 0;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  /*初始化*/
  ChassisInit();
  Lift_Init();
  Vision_Init();
  IMUInit();
  ElevatorMotor_Init();

  /*出来*/
  ChassisTransiation(Left, 30, (uint32_t)(21));
  ChassisTransiation(Forward, 40, (uint32_t)(60));
  CorrectError(0);
  // Lift_updown_control(down, 3500, 103000);
  // HAL_Delay(1000);

  /*问香橙派QRCode 阻塞*/
  while (command[3] != '+')
  {
  }

#if (QR_CODE_ADAPT == 1)
  /*二维码区微调*/
  HAL_UART_Transmit_DMA(&huart6, "2", 1);
  while (rangging[0] == 0x00)
  {
    HAL_UART_Transmit_DMA(&huart6, "2", 1);
    HAL_Delay(1200);
  }

  if (13 - d > 0)
  {
    ChassisTransiation(Left, 20, (uint32_t)(13 - d));
  }
  else
  {
    ChassisTransiation(Right, 20, (uint32_t)(d - 13));
  }
  HAL_Delay(500);
  HAL_UART_Transmit_DMA(&huart6, "0", 1);
  Lift_updown_control(up, 1000, 103000);
  HAL_Delay(1000);

#endif
  /*出发去转盘*/
  ChassisTransiation(Forward, 40, (uint32_t)(105));
  /*靠近转盘*/
  ChassisTransiation(Right, 30, (uint32_t)(8));
  CorrectError(0);
  //HAL_Delay(4000);
  Lift_StartFirst();

/*原料区微调*/
#if (RAW_ADAPT == 1)
  memset(input_copy, 0, sizeof(input_copy));
  while(1)
  {
    if(IsStable(0x33))
    {
      MicroAdapt_Goods(0x33,0,7);
      break;
    }else if(IsStable(0x34)){
      MicroAdapt_Goods(0x34,0,7);
      break;
    }else if(IsStable(0x35)){
      MicroAdapt_Goods(0x35,0,7);
      break;
    }
  }
                   
#endif
#if(LIFT == 1)
  /*在 物料稳定 并且 与当前要抓的匹配 时 抓取x3*/
  for (int i = 0; i < 3; i++)
  {
    while (!(IsMatch((char)(command[i]) + 2) && (IsStable((char)(command[i]) + 2))))
    {
    }
    Lift_Catch(element);
    num++;
    HAL_Delay(400);
    Lift_Back();
  }
  num++;//跳过指令中的加号

  Lift_Turn_back();
  putdown();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  /*离开转盘*/
  ChassisTransiation(Left, 30, (uint32_t)(7));

  /*出发去粗加工区*/
  ChassisTransiation(Back, 40, (uint32_t)(49));
  ChassisRotate(CounterClockWise_Chassis, 10, 115);
  HAL_Delay(2000);
  CorrectError(90);
  ChassisTransiation(Forward, 60, (uint32_t)(105));
  CorrectError(90);
  ChassisTransiation(Forward, 60, (uint32_t)(105));
  ChassisRotate(CounterClockWise_Chassis, 10, 115);
  HAL_Delay(2000);
  CorrectError(180);
#if(LIFT == 1)
  Lift_Turn();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif

  //将命令映射到色环区的坐标
  for (int i = 0; i < 3; i++)
  {
    if (command[i] == 0x31)
    {
      zone[i] = REDZONE;
    }
    else if (command[i] == 0x32)
    {
      zone[i] = GREENZONE;
    }
    else if (command[i] == 0x33)
    {
      zone[i] = BLUEZONE;
    }
  }
  ChassisTransiation(Right, 30, 19); // 靠近粗加工区

/*先校准绿色*/
#if (RING_ADAPT == 1)
  // memset(input_copy,0,sizeof(input_copy));

  // while (!Ring_IsStable((char)(0x31)))
  // {
  // }
  // MicroAdapt((char)0x31);

#endif
  /*第一个颜色区*/
  if (zone[0] > 0)
  {
    ChassisTransiation(Back, 30, (uint32_t)abs(zone[0]));
  }
  else
  {
    ChassisTransiation(Forward, 30, (uint32_t)abs(zone[0]));
  }
#if (RING_ADAPT == 1)
  //接收之前先清空拷贝的数组
  memset(input_copy, 0, sizeof(input_copy));
  while (!Ring_IsStable((char)(command[0]) - 1))
  {
  }
    MicroAdapt((char)(command[0]) - 1,8);
#endif
#if(LIFT == 1)
  Goods_Putdown(command[0]);
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  /*第二个颜色区*/
  if ((zone[1] - zone[0]) > 0)
  {
    ChassisTransiation(Back, 30, (uint32_t)(zone[1] - zone[0]));
  }
  else
  {
    ChassisTransiation(Forward, 30, (uint32_t)(zone[0] - zone[1]));
  }
#if (RING_ADAPT == 1)
  memset(input_copy, 0, sizeof(input_copy));
  while (!Ring_IsStable((char)(command[1]) - 1))
  {
  }
    MicroAdapt((char)(command[1]) - 1,4);

#endif
#if(LIFT == 1)
  Goods_Putdown(command[1]);
  Lift_Back();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  /*第三个颜色区*/
  if ((zone[2] - zone[1]) > 0)
  {
    ChassisTransiation(Back, 30, (uint32_t)(zone[2] - zone[1]));
  }
  else
  {
    ChassisTransiation(Forward, 30, (uint32_t)(zone[1] - zone[2]));
  }
#if (RING_ADAPT == 1)
  memset(input_copy, 0, sizeof(input_copy));
  while (!Ring_IsStable((char)(command[2]) - 1))
  {
  }
    MicroAdapt((char)(command[2]) - 1,4);
#endif
#if(LIFT == 1)
  Goods_Putdown(command[2]);
  Lift_Back();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  ChassisTransiation(Left,45,8);

  /*抓取*/
  // 第一区抓取
  if ((zone[0] - zone[2]) > 0)
  {
    ChassisTransiation(Back, 30, (uint32_t)(zone[0] - zone[2]));
  }
  else
  {
    ChassisTransiation(Forward, 30, (uint32_t)(zone[2] - zone[0]));
  }
#if (RING_ADAPT == 1)
  // memset(input_copy, 0, sizeof(input_copy));
  // while (!IsStable((char)(command[0]) - 1))
  // {
  // }
  // MicroAdapt((char)(command[0]) - 1);//+2

#endif
#if(LIFT == 1)
  Goods_Pickup(command[0]);
  Lift_Back();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  // 第二区抓取
  if ((zone[1] - zone[0]) > 0)
  {
    ChassisTransiation(Back, 30, (uint32_t)(zone[1] - zone[0]));
  }
  else
  {
    ChassisTransiation(Forward, 30, (uint32_t)(zone[0] - zone[1]));
  }
#if (RING_ADAPT == 1)
  // memset(input_copy, 0, sizeof(input_copy));

  // while (!IsStable((char)(command[1]) - 1))
  // {
  // }
  // MicroAdapt((char)(command[1]) - 1);

#endif
#if(LIFT == 1)
  Goods_Pickup(command[1]);
  Lift_Back();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  // 第三区抓取
  if ((zone[2] - zone[1]) > 0)
  {
    ChassisTransiation(Back, 30, (uint32_t)(zone[2] - zone[1]));
  }
  else
  {
    ChassisTransiation(Forward, 30, (uint32_t)(zone[1] - zone[2]));
  }
#if (RING_ADAPT == 1)
  // memset(input_copy, 0, sizeof(input_copy));
  // while (!IsStable((char)(command[2]) - 1))
  // {
  // }
  // MicroAdapt((char)(command[2]) - 1);

#endif
#if(LIFT == 1)
  Goods_Pickup(command[2]);
  Lift_Back();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  /*出发去暂存区*/
  ChassisTransiation(Left, 30, 10);
  CorrectError(180);
  ChassisTransiation(Back, 50, (uint32_t)(REDZONE - zone[2] + 80));
  ChassisRotate(ClockWise_Chassis, 10, 115);
  HAL_Delay(2000);
  CorrectError(90);
  ChassisTransiation(Back, 50, 95);

  /*靠近暂存区 第一个颜色区*/
  CorrectError(90);
  ChassisTransiation(Right, 30, 10);
  ChassisTransiation(Back, 30, (uint32_t)zone[0]);
#if (RING_ADAPT == 1)
  /*先校准绿色*/
  // while (!Ring_IsStable((char)(0x31)))
  // {
  // }
  // MicroAdapt((char)(0x31));
  memset(input_copy, 0, sizeof(input_copy));

  while (!Ring_IsStable((char)(command[0]) - 1))
  {
  }
  MicroAdapt((char)(command[0]) - 1,8);
#endif
#if(LIFT == 1)
  Goods_Putdown(command[0]);
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  /*第二个颜色区*/
  if ((zone[1] - zone[0]) > 0)
  {
    ChassisTransiation(Back, 30, (uint32_t)(zone[1] - zone[0]));
  }
  else
  {
    ChassisTransiation(Forward, 30, (uint32_t)(zone[0] - zone[1]));
  }
#if (RING_ADAPT == 1)
  memset(input_copy, 0, sizeof(input_copy));
  while (!Ring_IsStable((char)(command[1]) - 1))
  {
  }
  MicroAdapt((char)(command[1]) - 1,4);

#endif
#if(LIFT == 1)
  Goods_Putdown(command[1]);
  Lift_Back();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  /*第三个颜色区*/
  if ((zone[2] - zone[1]) > 0)
  {
    ChassisTransiation(Back, 30, (uint32_t)(zone[2] - zone[1]));
  }
  else
  {
    ChassisTransiation(Forward, 30, (uint32_t)(zone[1] - zone[2]));
  }
#if (RING_ADAPT == 1)
  memset(input_copy, 0, sizeof(input_copy));
  while (!Ring_IsStable((char)(command[2]) - 1))
  {
  }
  MicroAdapt((char)(command[2]) - 1,4);

#endif
#if(LIFT == 1)
  Goods_Putdown(command[2]);
  Lift_Back();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  ChassisTransiation(Left, 30, 20);

  /*-----------------------------开始第二圈-------------------------------------*/

  /*出发到原料区 */
  ChassisTransiation(Back, 50, (uint32_t)(REDZONE - zone[2] + 80));
  ChassisRotate(ClockWise_Chassis, 10, 115);
  HAL_Delay(2000);
  CorrectError(0);
  ChassisTransiation(Back, 40, 45);

  /*靠近转盘*/
  ChassisTransiation(Right, 30, (uint32_t)(12));
/*原料区微调*/
#if (RAW_ADAPT == 1)
  memset(input_copy, 0, sizeof(input_copy));
  while(1)
  {
    if(IsStable(0x33))
    {
      MicroAdapt_Goods(0x33,0,7);
      break;
    }else if(IsStable(0x34)){
      MicroAdapt_Goods(0x34,0,7);
      break;
    }else if(IsStable(0x35)){
      MicroAdapt_Goods(0x35,0,7);
      break;
    }
  }

#endif
#if(LIFT == 1)
  /*在 物料稳定 并且 与当前要抓的匹配 时 抓取x3*/
  // for (int i = 4; i < 7; i++)
  // {
  //   while (!(IsMatch((char)(command[i]) + 2) &&IsStable((char)(command[i]) + 2)))
  //   {
  //   }
  //   Lift_Catch(element);
  //   num++;
  //   HAL_Delay(400);
  //   Lift_Back();
  // }
    while (!(IsMatch((char)(command[4]) + 2) &&IsStable((char)(command[4]) + 2)))
    {
    }
    Lift_Catch(element);
    num++;
    HAL_Delay(400);
    Lift_Back();

  Lift_Turn_back();
  putdown();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  /*离开转盘*/
  ChassisTransiation(Left, 30, (uint32_t)(12));
  /*出发去粗加工区*/
  ChassisTransiation(Back, 40, (uint32_t)(47));
  ChassisRotate(CounterClockWise_Chassis, 10, 115);
  HAL_Delay(2000);
  CorrectError(90);
  ChassisTransiation(Forward, 60, (uint32_t)(100));
  CorrectError(90);
  ChassisTransiation(Forward, 60, (uint32_t)(105));
  ChassisRotate(CounterClockWise_Chassis, 10, 115);
  HAL_Delay(2000);
  CorrectError(180);
#if(LIFT == 1)
  Lift_Turn();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  for (int i = 4; i < 7; i++)
  {
    if (command[i] == 0x31)
    {
      zone[i] = REDZONE;
    }
    else if (command[i] == 0x32)
    {
      zone[i] = GREENZONE;
    }
    else if (command[i] == 0x33)
    {
      zone[i] = BLUEZONE;
    }
  }
  ChassisTransiation(Right, 30, 15); // 靠近粗加工区

/*先校准绿色*/
#if (RING_ADAPT == 1)
  // memset(input_copy,0,sizeof(input_copy));

  // while (!Ring_IsStable((char)(0x31)))
  // {
  // }
  // MicroAdapt((char)0x31);

#endif
  /*第一个颜色区*/
  if (zone[4] > 0)
  {
    ChassisTransiation(Back, 30, (uint32_t)abs(zone[4]));
  }
  else
  {
    ChassisTransiation(Forward, 30, (uint32_t)abs(zone[4]));
  }
#if (RING_ADAPT == 1)
  //接收之前先清空拷贝的数组
  memset(input_copy, 0, sizeof(input_copy));
  while (!Ring_IsStable((char)(command[4]) - 1))
  {
  }
  MicroAdapt((char)(command[4]) - 1,8);
#endif
#if(LIFT == 1)
  Goods_Putdown(command[4]);
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  /*第二个颜色区*/
  if ((zone[5] - zone[4]) > 0)
  {
    ChassisTransiation(Back, 30, (uint32_t)(zone[5] - zone[4]));
  }
  else
  {
    ChassisTransiation(Forward, 30, (uint32_t)(zone[4] - zone[5]));
  }
#if (RING_ADAPT == 1)
  memset(input_copy, 0, sizeof(input_copy));
  while (!Ring_IsStable((char)(command[5]) - 1))
  {
  }
  MicroAdapt((char)(command[5]) - 1,4);

#endif
#if(LIFT == 1)
  Goods_Putdown(command[5]);
  Lift_Back();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  /*第三个颜色区*/
  if ((zone[6] - zone[5]) > 0)
  {
    ChassisTransiation(Back, 30, (uint32_t)(zone[6] - zone[5]));
  }
  else
  {  
    ChassisTransiation(Forward, 30, (uint32_t)(zone[5] - zone[6]));
  }
#if (RING_ADAPT == 1)
  memset(input_copy, 0, sizeof(input_copy));
  while (!Ring_IsStable((char)(command[6]) - 1))
  {
  }
  MicroAdapt((char)(command[6]) - 1,4);
#endif
#if(LIFT == 1)
  Goods_Putdown(command[6]);
  Lift_Back();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  ChassisTransiation(Left,45,10);
  /*抓取*/
  // 第一区抓取
  if ((zone[4] - zone[6]) > 0)
  {
    ChassisTransiation(Back, 30, (uint32_t)(zone[4] - zone[6]));
  }
  else
  {
    ChassisTransiation(Forward, 30, (uint32_t)(zone[6] - zone[4]));
  }
#if (RING_ADAPT == 1)
  // memset(input_copy, 0, sizeof(input_copy));

  // while (!IsStable((char)(command[4]) - 1))
  // {
  // }
  // MicroAdapt((char)(command[4]) - 1);//+2

#endif
#if(LIFT == 1)
  Goods_Pickup(command[4]);
  Lift_Back();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  // 第二区抓取
  if ((zone[5] - zone[4]) > 0)
  {
    ChassisTransiation(Back, 30, (uint32_t)(zone[5] - zone[4]));
  }
  else
  {
    ChassisTransiation(Forward, 30, (uint32_t)(zone[4] - zone[5]));
  }
#if (RING_ADAPT == 1)
  // memset(input_copy, 0, sizeof(input_copy));

  // while (!IsStable((char)(command[5]) - 1))
  // {
  // }
  // MicroAdapt((char)(command[5]) - 1);

#endif
#if(LIFT == 1)
  Goods_Pickup(command[5]);
  Lift_Back();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  // 第三区抓取
  if ((zone[6] - zone[5]) > 0)
  {
    ChassisTransiation(Back, 30, (uint32_t)(zone[6] - zone[5]));
  }
  else
  {
    ChassisTransiation(Forward, 30, (uint32_t)(zone[5] - zone[6]));
  }
#if (RING_ADAPT == 1)
  // memset(input_copy, 0, sizeof(input_copy));

  // while (!IsStable((char)(command[6]) - 1))
  // {
  // }
  // MicroAdapt((char)(command[6]) - 1);

#endif
#if(LIFT == 1)
  Goods_Pickup(command[6]);
  Lift_Back();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  /*出发去暂存区*/
  ChassisTransiation(Left, 30, 10);
  CorrectError(180);
  ChassisTransiation(Back, 50, (uint32_t)(REDZONE - zone[6] + 80));
  ChassisRotate(ClockWise_Chassis, 10, 115);
  HAL_Delay(2000);
  CorrectError(90);
  ChassisTransiation(Back, 50, 95);

  /*靠近暂存区 第一个颜色区*/
  CorrectError(90);
  ChassisTransiation(Right, 30, 10);
  ChassisTransiation(Back, 30, (uint32_t)zone[4]);
#if (RING_ADAPT == 1)
  /*先校准绿色*/
  // while (!Ring_IsStable((char)(0x31)))
  // {
  // }
  // MicroAdapt((char)(0x31));
  memset(input_copy, 0, sizeof(input_copy));

  while (!Ring_IsStable((char)(command[4]) - 1))
  {
  }
  MicroAdapt((char)(command[4]) - 1,8);
#endif
#if(LIFT == 1)
  Modes_Putdown(command[4]);
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  /*第二个颜色区*/
  if ((zone[5] - zone[4]) > 0)
  {
    ChassisTransiation(Back, 30, (uint32_t)(zone[5] - zone[4]));
  }
  else
  {
    ChassisTransiation(Forward, 30, (uint32_t)(zone[4] - zone[5]));
  }
#if (RING_ADAPT == 1)
  memset(input_copy, 0, sizeof(input_copy));
  while (!Ring_IsStable((char)(command[5]) - 1))
  {
  }
  MicroAdapt((char)(command[5]) - 1,4);

#endif
#if(LIFT == 1)
  Modes_Putdown(command[5]);
  Lift_Back();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  /*第三个颜色区*/
  if ((zone[6] - zone[5]) > 0)
  {
    ChassisTransiation(Back, 30, (uint32_t)(zone[6] - zone[5]));
  }
  else
  {
    ChassisTransiation(Forward, 30, (uint32_t)(zone[5] - zone[6]));
  }
#if (RING_ADAPT == 1)
  memset(input_copy, 0, sizeof(input_copy));
  while (!Ring_IsStable((char)(command[6]) - 1))
  {
  }
  MicroAdapt((char)(command[6]) - 1,4);

#endif
#if(LIFT == 1)
  Modes_Putdown(command[6]);
  Lift_Back();
#endif
#if(LIFT == 0)
  HAL_Delay(1000);
#endif
  ChassisTransiation(Left, 30, 10);

  /*出发到原料区 */
  ChassisTransiation(Back, 50, (uint32_t)(REDZONE - zone[6] + 80));
  ChassisRotate(ClockWise_Chassis, 10, 115);
  HAL_Delay(2000);
  CorrectError(0);
  ChassisTransiation(Back, 40, 207);
  ChassisTransiation(RightBack, 30, 16);
}

/* USER CODE END 2 */

/* Infinite loop */
/* USER CODE BEGIN WHILE */
// while (1)
// {

//   /* USER CODE END WHILE */
//   /* USER CODE BEGIN 3 */
// }
/* USER CODE END 3 */

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
