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
#include "StepMotor.h"
#include "Chassis.h"
#include "ServoMotor.h"
#include "Lift.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "IMU.h"
#include "StepMotor.h"
#include "Chassis.h"
#include "Vision.h"
#include "bsp_usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RANGING           0                               //测距矫正的开关宏定义
#define REDZONE           15
#define GREENZONE         0
#define BLUEZONE          -15
#define TURN_K            0.8
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
  int zone[3];
  int x,y;
  
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void CorrectError(float TargetAngle)
{
    IMURecive();
    float error,Last_Yaw_t=0;
    error = TargetAngle-Yaw_t;
    if(error>=1||error<=-1)
    {
      if(error>0)
      {
        ChassisRotate(CounterClockWise_Chassis,10,TURN_K*error);
        HAL_Delay(1000);
      }else{
        ChassisRotate(ClockWise_Chassis,10,-TURN_K*error);
        HAL_Delay(1000);
      }
      Yaw_t = 0;
    }else{
        return 0;
        Yaw_t = 0;
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
  command[7]=1;
  command[0]=0x31;
  command[1]=0x32;
  command[2]=0x33;
  command[4]=0x33;
  command[5]=0x32;
  command[6]=0x31;

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
  ChassisTransiation(Left,20,(uint32_t)(23));
  ChassisTransiation(Forward,30,(uint32_t)(75));
  CorrectError(0);
  Lift_StartFirst();
  Lift_updown_control(down,1000,100000);
  HAL_Delay(3000);
  
  /*问香橙派QRCode 阻塞*/
  while(command[3]!='+'){}

  //HAL_Delay(1000);

  /*二维码区微调*/
  HAL_UART_Transmit_DMA(&huart6,"2",1);
  while(rangging[0]==0x00)
  {
      HAL_UART_Transmit_DMA(&huart6,"2",1);
      HAL_Delay(1500);
  }
  if(13-d>0)
  {
    ChassisTransiation(Left,20,(uint32_t)(13-d));
  }else
  {
    ChassisTransiation(Right,20,(uint32_t)(d-13));
  }
  HAL_UART_Transmit_DMA(&huart6,"0",1);
  Lift_updown_control(up,1000,100000);
  HAL_Delay(3000);

  /*出发去转盘*/
  ChassisTransiation(Forward,30,(uint32_t)(90));
   /*靠近转盘*/
   ChassisTransiation(Right,20,(uint32_t)(9));
   CorrectError(0);

  /*原料区微调*/
    // while(!IsStable()){}
    // if(!((abs(300-x_int)<=20)&&(abs(200-y_int)<=15)))//320 250
    // {
    //     if(300-x_int<0)
    //     {
    //       ChassisTransiation(Back,20,(uint32_t)(x_int-300));
    //     }else if(300-x_int>0)
    //     {
    //       ChassisTransiation(Forward,20,(uint32_t)(300-x_int));
    //     }
    //     if(200-y_int<0)
    //     {
    //       ChassisTransiation(Left,20,(uint32_t)(y_int-200));
    //     }else if(200-y_int>0)
    //     {
    //       ChassisTransiation(Right,20,(uint32_t)(200-y_int));
    //     }
    // }

    /*在 物料稳定 并且 与当前要抓的匹配 时 抓取x3*/
   for(int i = 0;i < 3;i++)
   {
       while(!(IsStable() && IsMatch())){}
       Lift_Catch(element);
       HAL_Delay(1000);
       Lift_Back();
    }
    Lift_Turn_back();
    putdown();
    HAL_Delay(1000);

  /*离开转盘*/
   ChassisTransiation(Left,20,(uint32_t)(9));
  /*出发去粗加工区*/
   ChassisTransiation(Back,30,(uint32_t)(46));
   ChassisRotate(CounterClockWise_Chassis,10,115);
   HAL_Delay(2000);
   CorrectError(90);
   ChassisTransiation(Forward,50,(uint32_t)(190));
   ChassisRotate(CounterClockWise_Chassis,10,115);
   HAL_Delay(2000);
   CorrectError(180);
   Lift_Turn();
  for(int i = 0;i < 3;i++)
  {
    if(command[i]==0x31)
    {
      zone[i]=REDZONE;
    }else if(command[i]==0x32)
    {
      zone[i]=GREENZONE;
    }else if(command[i]==0x33)
    {
      zone[i]=BLUEZONE;
    }
  }
  ChassisTransiation(Right,20,10);

  /*先校准绿色*/
    while(!IsStable()){}
    while(1)
    {
      if(wuliao[0]==0x31)
      {
        if(!((abs(300-x_int)<=20)&&(abs(200-y_int)<=15)))//320 250
        {
            if(300-x_int<0)
            {
              ChassisTransiation(Back,20,(uint32_t)(0.01*(x_int-300)));
              break;
            }else if(300-x_int>0)
            {
              ChassisTransiation(Forward,20,(uint32_t)(0.01*(300-x_int)));
              break;
            }
            if(200-y_int<0)
            {
              ChassisTransiation(Left,20,(uint32_t)(0.01*(y_int-200)));
              break;
            }else if(200-y_int>0)
            {
              ChassisTransiation(Right,20,(uint32_t)(0.01*(200-y_int)));
              break;
            }
        }
      }
    }

  /*靠近粗加工区 第一个颜色区*/
   if(zone[0]>0)
   {
      ChassisTransiation(Back,20,(uint32_t)zone[0]);
   }else{
      ChassisTransiation(Forward,20,(uint32_t)abs(zone[0]));
   }

    // while(!IsStable()){}
    // if(!((abs(300-x_int)<=20)&&(abs(200-y_int)<=15)))//320 250
    // {
    //     if(300-x_int<0)
    //     {
    //       ChassisTransiation(Back,20,(uint32_t)(0.01*(x_int-300)));
    //     }else if(300-x_int>0)
    //     {
    //       ChassisTransiation(Forward,20,(uint32_t)(0.01*(300-x_int)));
    //     }
    //     if(200-y_int<0)
    //     {
    //       ChassisTransiation(Left,20,(uint32_t)(0.01*(y_int-200)));
    //     }else if(200-y_int>0)
    //     {
    //       ChassisTransiation(Right,20,(uint32_t)(0.01*(200-y_int)));
    //     }
    // }

  // Goods_Putdown(command[0]);
  /*第二个颜色区*/
  if((zone[1]-zone[0])>0)
  {
    ChassisTransiation(Back,20,(uint32_t)(zone[1]-zone[0]));   
  }else{
    ChassisTransiation(Forward,20,(uint32_t)(zone[0]-zone[1]));
  }
    // while(!IsStable()){}
    // if(!((abs(300-x_int)<=20)&&(abs(200-y_int)<=15)))//320 250
    // {
    //     if(300-x_int<0)
    //     {
    //       ChassisTransiation(Back,20,(uint32_t)(x_int-300));
    //     }else if(300-x_int>0)
    //     {
    //       ChassisTransiation(Forward,20,(uint32_t)(300-x_int));
    //     }
    //     if(200-y_int<0)
    //     {
    //       ChassisTransiation(Left,20,(uint32_t)(y_int-200));
    //     }else if(200-y_int>0)
    //     {
    //       ChassisTransiation(Right,20,(uint32_t)(200-y_int));
    //     }
    // }

  Goods_Putdown(command[1]);
  Lift_Back();
  /*第三个颜色区*/
  if((zone[2]-zone[1])>0)
  {
    ChassisTransiation(Back,20,(uint32_t)(zone[2]-zone[1]));
  }else{
    ChassisTransiation(Forward,20,(uint32_t)(zone[1]-zone[2]));
  }
    // while(!IsStable()){}
    // if(!((abs(300-x_int)<=20)&&(abs(200-y_int)<=15)))//320 250
    // {
    //     if(300-x_int<0)
    //     {
    //       ChassisTransiation(Back,20,(uint32_t)(x_int-300));
    //     }else if(300-x_int>0)
    //     {
    //       ChassisTransiation(Forward,20,(uint32_t)(300-x_int));
    //     }
    //     if(200-y_int<0)
    //     {
    //       ChassisTransiation(Left,20,(uint32_t)(y_int-200));
    //     }else if(200-y_int>0)
    //     {
    //       ChassisTransiation(Right,20,(uint32_t)(200-y_int));
    //     }
    // }

  Goods_Putdown(command[2]);
  Lift_Back();


/*抓取*/
//第一区抓取
  if((zone[0]-zone[2])>0)
  {
    ChassisTransiation(Back,20,(uint32_t)(zone[0]-zone[2]));   
  }else{
    ChassisTransiation(Forward,20,(uint32_t)(zone[2]-zone[0]));
  }
    while(!IsStable()){}
    if(!((abs(300-x_int)<=20)&&(abs(200-y_int)<=15)))//320 250
    {
        if(300-x_int<0)
        {
          ChassisTransiation(Back,20,(uint32_t)(x_int-300));
        }else if(300-x_int>0)
        {
          ChassisTransiation(Forward,20,(uint32_t)(300-x_int));
        }
        if(200-y_int<0)
        {
          ChassisTransiation(Left,20,(uint32_t)(y_int-200));
        }else if(200-y_int>0)
        {
          ChassisTransiation(Right,20,(uint32_t)(200-y_int));
        }
    }

  Goods_Pickup(command[0]);
  Lift_Back();

//第二区抓取
  if((zone[1]-zone[0])>0)
  {
    ChassisTransiation(Back,20,(uint32_t)(zone[1]-zone[0]));   
  }else{
    ChassisTransiation(Forward,20,(uint32_t)(zone[0]-zone[1]));
  }
    while(!IsStable()){}
    if(!((abs(300-x_int)<=20)&&(abs(200-y_int)<=15)))//320 250
    {
        if(300-x_int<0)
        {
          ChassisTransiation(Back,20,(uint32_t)(x_int-300));
        }else if(300-x_int>0)
        {
          ChassisTransiation(Forward,20,(uint32_t)(300-x_int));
        }
        if(200-y_int<0)
        {
          ChassisTransiation(Left,20,(uint32_t)(y_int-200));
        }else if(200-y_int>0)
        {
          ChassisTransiation(Right,20,(uint32_t)(200-y_int));
        }
    }

  Goods_Pickup(command[1]);
  Lift_Back();

//第三区抓取
  if((zone[2]-zone[1])>0)
  {
    ChassisTransiation(Back,20,(uint32_t)(zone[2]-zone[1]));   
  }else{
    ChassisTransiation(Forward,20,(uint32_t)(zone[1]-zone[2]));
  }
    while(!IsStable()){}
    if(!((abs(300-x_int)<=20)&&(abs(200-y_int)<=15)))//320 250
    {
        if(300-x_int<0)
        {
          ChassisTransiation(Back,20,(uint32_t)(x_int-300));
        }else if(300-x_int>0)
        {
          ChassisTransiation(Forward,20,(uint32_t)(300-x_int));
        }
        if(200-y_int<0)
        {
          ChassisTransiation(Left,20,(uint32_t)(y_int-200));
        }else if(200-y_int>0)
        {
          ChassisTransiation(Right,20,(uint32_t)(200-y_int));
        }
    }

  Goods_Pickup(command[2]);
  Lift_Back();
   
  /*出发去暂存区*/
   ChassisTransiation(Left,20,10);
   CorrectError(180);
   ChassisTransiation(Back,40,(uint32_t)(REDZONE-zone[2]+75));
   ChassisRotate(ClockWise_Chassis,10,115);
   HAL_Delay(2000);
   CorrectError(90);
   ChassisTransiation(Back,40,115);
  /*靠近暂存区 第一个颜色区*/
   CorrectError(90);
   ChassisTransiation(Right,20,10);
   ChassisTransiation(Back,20,(uint32_t)zone[0]);
    while(!IsStable()){}
    if(!((abs(300-x_int)<=20)&&(abs(200-y_int)<=15)))//320 250
    {
        if(300-x_int<0)
        {
          ChassisTransiation(Back,20,(uint32_t)(x_int-300));
        }else if(300-x_int>0)
        {
          ChassisTransiation(Forward,20,(uint32_t)(300-x_int));
        }
        if(200-y_int<0)
        {
          ChassisTransiation(Left,20,(uint32_t)(y_int-200));
        }else if(200-y_int>0)
        {
          ChassisTransiation(Right,20,(uint32_t)(200-y_int));
        }
    }

  Goods_Putdown(command[0]);
  /*第二个颜色区*/
  if((zone[1]-zone[0])>0)
  {
    ChassisTransiation(Back,20,(uint32_t)(zone[1]-zone[0]));   
  }else{
    ChassisTransiation(Forward,20,(uint32_t)(zone[0]-zone[1]));
  }
  while(!(IsStable())){}
  x = x_error();
  y = y_error();

  if(!((abs(320-x)<=10)&&(abs(350-y)<=8)))
  {
    if(320-x<0)
    {
      ChassisTransiation(Back,20,(uint32_t)(MU*K*(x-320)));
      HAL_Delay((uint32_t)(T*MU*K*(x-320)/20));
    }else if(320-x>0)
    {
      ChassisTransiation(Forward,20,(uint32_t)(MU*K*(320-x)));
      HAL_Delay((uint32_t)(T*MU*K*(320-x)/20));
    }else if(350-y<0)
    {
      ChassisTransiation(Left,20,(uint32_t)(MU*K*(y-350)));
      HAL_Delay((uint32_t)(T*MU*K*(y-350)/20));
    }else if(350-y>0)
    {
      ChassisTransiation(Right,20,(uint32_t)(MU*K*(350-y)));
      HAL_Delay((uint32_t)(T*MU*K*(350-y)/20));
    }
  }

  Goods_Putdown(command[1]);
  Lift_Back();
  /*第三个颜色区*/
  if((zone[2]-zone[1])>0)
  {
    ChassisTransiation(Back,20,(uint32_t)(zone[2]-zone[1]));
  }else{
    ChassisTransiation(Forward,20,(uint32_t)(zone[1]-zone[2]));
  }
  while(!(IsStable())){}
  x = x_error();
  y = y_error();

  if(!((abs(320-x)<=10)&&(abs(350-y)<=8)))
  {
    if(320-x<0)
    {
      ChassisTransiation(Back,20,(uint32_t)(MU*K*(x-320)));
      HAL_Delay((uint32_t)(T*MU*K*(x-320)/20));
    }else if(320-x>0)
    {
      ChassisTransiation(Forward,20,(uint32_t)(MU*K*(320-x)));
      HAL_Delay((uint32_t)(T*MU*K*(320-x)/20));
    }else if(350-y<0)
    {
      ChassisTransiation(Left,20,(uint32_t)(MU*K*(y-350)));
      HAL_Delay((uint32_t)(T*MU*K*(y-350)/20));
    }else if(350-y>0)
    {
      ChassisTransiation(Right,20,(uint32_t)(MU*K*(350-y)));
      HAL_Delay((uint32_t)(T*MU*K*(350-y)/20));
    }
  }

  Goods_Putdown(command[2]);
  Lift_Back();


/*----------开始第二圈---------------*/

/*出发到原料区 */
   ChassisTransiation(Back,40,(uint32_t)(REDZONE-zone[2]+75));
   ChassisRotate(ClockWise_Chassis,10,115);
   HAL_Delay(2000);
   CorrectError(0);
   ChassisTransiation(Back,30,30);

  /*靠近转盘*/
   ChassisTransiation(Right,20,(uint32_t)(7));
  /*在 物料稳定 并且 与当前要抓的匹配 时 抓取x3*/
  //  for(int i = 0;i < 3;i++)
  //  {
  //      while(!(IsStable() && IsMatch())){}
  //     Lift_Catch(element);
  //     //HAL_Delay(1000);
  //     Lift_Back();
  //  }
  // for(int i = 0;i < 3;i++)
  // {
  //     while(!(IsStable() && IsMatch())){}
  //     x = x_error();
  //     y = y_error();
  //     if(!((abs(320-x)<=20)&&(abs(350-y)<=15)))
  //     {
  //       if(320-x<0)
  //       {
  //         ChassisTransiation(Back,20,(uint32_t)(MU*K*(x-320)));
  //         HAL_Delay((uint32_t)(T*MU*K*(x-320)/20));
  //       }else if(320-x>0)
  //       {
  //         ChassisTransiation(Forward,20,(uint32_t)(MU*K*(320-x)));
  //         HAL_Delay((uint32_t)(T*MU*K*(320-x)/20));
  //       }else if(350-y<0)
  //       {
  //         ChassisTransiation(Left,20,(uint32_t)(MU*K*(y-350)));
  //         HAL_Delay((uint32_t)(T*MU*K*(y-350)/20));
  //       }else if(350-y>0)
  //       {
  //         ChassisTransiation(Right,20,(uint32_t)(MU*K*(350-y)));
  //         HAL_Delay((uint32_t)(T*MU*K*(350-y)/20));
  //       }
  //     }
  //  }
  // Lift_Catch(element);

  /*离开转盘*/
   ChassisTransiation(Left,20,(uint32_t)(7));
  /*出发去粗加工区*/
   ChassisTransiation(Back,30,(uint32_t)(30));
   ChassisRotate(CounterClockWise_Chassis,10,115);
   HAL_Delay(2000);
   CorrectError(90);
   ChassisTransiation(Forward,50,(uint32_t)(190));
   ChassisRotate(CounterClockWise_Chassis,10,115);
   HAL_Delay(2000);
   CorrectError(180);
  for(int i = 4;i < 7;i++)
  {
    if(command[i]==0x31)
    {
      zone[i]=REDZONE;
    }else if(command[i]==0x32)
    {
      zone[i]=GREENZONE;
    }else if(command[i]==0x33)
    {
      zone[i]=BLUEZONE;
    }
  }
  /*靠近粗加工区 第一个颜色区*/
   ChassisTransiation(Right,20,10);
   if(zone[0]>0)
   {
      ChassisTransiation(Back,20,(uint32_t)zone[0]);
   }else{
      ChassisTransiation(Forward,20,(uint32_t)abs(zone[0]));
   }

  //  while(!(IsStable())){}
  //  x = x_error();
  //  y = y_error();

  // if(!((abs(320-x)<=10)&&(abs(350-y)<=8)))
  // {
  //   if(320-x<0)
  //   {
  //     ChassisTransiation(Back,20,(uint32_t)(MU*K*(x-320)));
  //     HAL_Delay((uint32_t)(T*MU*K*(x-320)/20));
  //   }else if(320-x>0)
  //   {
  //     ChassisTransiation(Forward,20,(uint32_t)(MU*K*(320-x)));
  //     HAL_Delay((uint32_t)(T*MU*K*(320-x)/20));
  //   }else if(350-y<0)
  //   {
  //     ChassisTransiation(Left,20,(uint32_t)(MU*K*(y-350)));
  //     HAL_Delay((uint32_t)(T*MU*K*(y-350)/20));
  //   }else if(350-y>0)
  //   {
  //     ChassisTransiation(Right,20,(uint32_t)(MU*K*(350-y)));
  //     HAL_Delay((uint32_t)(T*MU*K*(350-y)/20));
  //   }
  // }

  // Goods_Putdown(command[0]);
  /*第二个颜色区*/
  if((zone[1]-zone[0])>0)
  {
    ChassisTransiation(Back,20,(uint32_t)(zone[1]-zone[0]));   
  }else{
    ChassisTransiation(Forward,20,(uint32_t)(zone[0]-zone[1]));
  }
  // while(!(IsStable())){}
  // x = x_error();
  // y = y_error();

  // if(!((abs(320-x)<=10)&&(abs(350-y)<=8)))
  // {
  //   if(320-x<0)
  //   {
  //     ChassisTransiation(Back,20,(uint32_t)(MU*K*(x-320)));
  //     HAL_Delay((uint32_t)(T*MU*K*(x-320)/20));
  //   }else if(320-x>0)
  //   {
  //     ChassisTransiation(Forward,20,(uint32_t)(MU*K*(320-x)));
  //     HAL_Delay((uint32_t)(T*MU*K*(320-x)/20));
  //   }else if(350-y<0)
  //   {
  //     ChassisTransiation(Left,20,(uint32_t)(MU*K*(y-350)));
  //     HAL_Delay((uint32_t)(T*MU*K*(y-350)/20));
  //   }else if(350-y>0)
  //   {
  //     ChassisTransiation(Right,20,(uint32_t)(MU*K*(350-y)));
  //     HAL_Delay((uint32_t)(T*MU*K*(350-y)/20));
  //   }
  // }

  // Goods_Putdown(command[1]);
  //Lift_Back();
  /*第三个颜色区*/
  if((zone[2]-zone[1])>0)
  {
    ChassisTransiation(Back,20,(uint32_t)(zone[2]-zone[1]));
  }else{
    ChassisTransiation(Forward,20,(uint32_t)(zone[1]-zone[2]));
  }
  // while(!(IsStable())){}
  // x = x_error();
  // y = y_error();

  // if(!((abs(320-x)<=10)&&(abs(350-y)<=8)))
  // {
  //   if(320-x<0)
  //   {
  //     ChassisTransiation(Back,20,(uint32_t)(MU*K*(x-320)));
  //     HAL_Delay((uint32_t)(T*MU*K*(x-320)/20));
  //   }else if(320-x>0)
  //   {
  //     ChassisTransiation(Forward,20,(uint32_t)(MU*K*(320-x)));
  //     HAL_Delay((uint32_t)(T*MU*K*(320-x)/20));
  //   }else if(350-y<0)
  //   {
  //     ChassisTransiation(Left,20,(uint32_t)(MU*K*(y-350)));
  //     HAL_Delay((uint32_t)(T*MU*K*(y-350)/20));
  //   }else if(350-y>0)
  //   {
  //     ChassisTransiation(Right,20,(uint32_t)(MU*K*(350-y)));
  //     HAL_Delay((uint32_t)(T*MU*K*(350-y)/20));
  //   }
  // }

  // Goods_Putdown(command[2]);
  // Lift_Back();


/*抓取*/
//第一区抓取
  if((zone[0]-zone[2])>0)
  {
    ChassisTransiation(Back,20,(uint32_t)(zone[0]-zone[2]));   
  }else{
    ChassisTransiation(Forward,20,(uint32_t)(zone[2]-zone[0]));
  }
  // while(!(IsStable())){}
  // x = x_error();
  // y = y_error();

  // if(!((abs(320-x)<=10)&&(abs(350-y)<=8)))
  // {
  //   if(320-x<0)
  //   {
  //     ChassisTransiation(Back,20,(uint32_t)(MU*K*(x-320)));
  //     HAL_Delay((uint32_t)(T*MU*K*(x-320)/20));
  //   }else if(320-x>0)
  //   {
  //     ChassisTransiation(Forward,20,(uint32_t)(MU*K*(320-x)));
  //     HAL_Delay((uint32_t)(T*MU*K*(320-x)/20));
  //   }else if(350-y<0)
  //   {
  //     ChassisTransiation(Left,20,(uint32_t)(MU*K*(y-350)));
  //     HAL_Delay((uint32_t)(T*MU*K*(y-350)/20));
  //   }else if(350-y>0)
  //   {
  //     ChassisTransiation(Right,20,(uint32_t)(MU*K*(350-y)));
  //     HAL_Delay((uint32_t)(T*MU*K*(350-y)/20));
  //   }
  // }

  // Goods_Pickup(command[0]);
  // Lift_Back();

//第二区抓取
  if((zone[1]-zone[0])>0)
  {
    ChassisTransiation(Back,20,(uint32_t)(zone[1]-zone[0]));
  }else{
    ChassisTransiation(Forward,20,(uint32_t)(zone[0]-zone[1]));
  }
  // while(!(IsStable())){}
  // x = x_error();
  // y = y_error();

  // if(!((abs(320-x)<=10)&&(fabs(350-y)<=8)))
  // {
  //   if(320-x<0)
  //   {
  //     ChassisTransiation(Back,20,(uint32_t)(MU*K*(x-320)));
  //     HAL_Delay((uint32_t)(T*MU*K*(x-320)/20));
  //   }else if(320-x>0)
  //   {
  //     ChassisTransiation(Forward,20,(uint32_t)(MU*K*(320-x)));
  //     HAL_Delay((uint32_t)(T*MU*K*(320-x)/20));
  //   }else if(350-y<0)
  //   {
  //     ChassisTransiation(Left,20,(uint32_t)(MU*K*(y-350)));
  //     HAL_Delay((uint32_t)(T*MU*K*(y-350)/20));
  //   }else if(350-y>0)
  //   {
  //     ChassisTransiation(Right,20,(uint32_t)(MU*K*(350-y)));
  //     HAL_Delay((uint32_t)(T*MU*K*(350-y)/20));
  //   }
  // }

  // Goods_Pickup(command[1]);
  // Lift_Back();

//第三区抓取
  if((zone[2]-zone[1])>0)
  {
    ChassisTransiation(Back,20,(uint32_t)(zone[2]-zone[1]));   
  }else{
    ChassisTransiation(Forward,20,(uint32_t)(zone[1]-zone[2]));
  }
  CorrectError(180);
  // while(!(IsStable())){}
  // x = x_error();
  // y = y_error();

  // if(!((abs(320-x)<=10)&&(abs(350-y)<=8)))
  // {
  //   if(320-x<0)
  //   {
  //     ChassisTransiation(Back,20,(uint32_t)(MU*K*(x-320)));
  //     HAL_Delay((uint32_t)(T*MU*K*(x-320)/20));
  //   }else if(320-x>0)
  //   {
  //     ChassisTransiation(Forward,20,(uint32_t)(MU*K*(320-x)));
  //     HAL_Delay((uint32_t)(T*MU*K*(320-x)/20));
  //   }else if(350-y<0)
  //   {
  //     ChassisTransiation(Left,20,(uint32_t)(MU*K*(y-350)));
  //     HAL_Delay((uint32_t)(T*MU*K*(y-350)/20));
  //   }else if(350-y>0)
  //   {
  //     ChassisTransiation(Right,20,(uint32_t)(MU*K*(350-y)));
  //     HAL_Delay((uint32_t)(T*MU*K*(350-y)/20));
  //   }
  // }

  // Goods_Pickup(command[2]);
  // Lift_Back();
   
  /*出发去暂存区*/
   ChassisTransiation(Left,20,8);
   ChassisTransiation(Back,40,(uint32_t)(REDZONE-zone[2]+75));
   ChassisRotate(ClockWise_Chassis,10,115);
   HAL_Delay(2000);
   CorrectError(90);
   ChassisTransiation(Back,40,75);
  /*靠近暂存区
   第一个颜色区*/
   ChassisTransiation(Right,20,10);
   ChassisTransiation(Back,20,(uint32_t)zone[0]);
  // while(!(IsStable())){}
  // x = x_error();
  // y = y_error();

  // if(!((abs(320-x)<=10)&&(abs(350-y)<=8)))
  // {
  //   if(320-x<0)
  //   {
  //     ChassisTransiation(Back,20,(uint32_t)(MU*K*(x-320)));
  //     HAL_Delay((uint32_t)(T*MU*K*(x-320)/20));
  //   }else if(320-x>0)
  //   {
  //     ChassisTransiation(Forward,20,(uint32_t)(MU*K*(320-x)));
  //     HAL_Delay((uint32_t)(T*MU*K*(320-x)/20));
  //   }else if(350-y<0)
  //   {
  //     ChassisTransiation(Left,20,(uint32_t)(MU*K*(y-350)));
  //     HAL_Delay((uint32_t)(T*MU*K*(y-350)/20));
  //   }else if(350-y>0)
  //   {
  //     ChassisTransiation(Right,20,(uint32_t)(MU*K*(350-y)));
  //     HAL_Delay((uint32_t)(T*MU*K*(350-y)/20));
  //   }
  // }

  // Goods_Putdown(command[0]);
  /*第二个颜色区*/
  if((zone[1]-zone[0])>0)
  {
    ChassisTransiation(Back,20,(uint32_t)(zone[1]-zone[0]));   
  }else{
    ChassisTransiation(Forward,20,(uint32_t)(zone[0]-zone[1]));
  }
  // while(!(IsStable())){}
  // x = x_error();
  // y = y_error();

  // if(!((abs(320-x)<=10)&&(abs(350-y)<=8)))
  // {
  //   if(320-x<0)
  //   {
  //     ChassisTransiation(Back,20,(uint32_t)(MU*K*(x-320)));
  //     HAL_Delay((uint32_t)(T*MU*K*(x-320)/20));
  //   }else if(320-x>0)
  //   {
  //     ChassisTransiation(Forward,20,(uint32_t)(MU*K*(320-x)));
  //     HAL_Delay((uint32_t)(T*MU*K*(320-x)/20));
  //   }else if(350-y<0)
  //   {
  //     ChassisTransiation(Left,20,(uint32_t)(MU*K*(y-350)));
  //     HAL_Delay((uint32_t)(T*MU*K*(y-350)/20));
  //   }else if(350-y>0)
  //   {
  //     ChassisTransiation(Right,20,(uint32_t)(MU*K*(350-y)));
  //     HAL_Delay((uint32_t)(T*MU*K*(350-y)/20));
  //   }
  // }

  // Goods_Putdown(command[1]);
  // Lift_Back();
  /*第三个颜色区*/
  if((zone[2]-zone[1])>0)
  {
    ChassisTransiation(Back,20,(uint32_t)(zone[2]-zone[1]));
  }else{
    ChassisTransiation(Forward,20,(uint32_t)(zone[1]-zone[2]));
  }
  // while(!(IsStable())){}
  // x = x_error();
  // y = y_error();

  // if(!((abs(320-x)<=10)&&(abs(350-y)<=8)))
  // {
  //   if(320-x<0)
  //   {
  //     ChassisTransiation(Back,20,(uint32_t)(MU*K*(x-320)));
  //     HAL_Delay((uint32_t)(T*MU*K*(x-320)/20));
  //   }else if(320-x>0)
  //   {
  //     ChassisTransiation(Forward,20,(uint32_t)(MU*K*(320-x)));
  //     HAL_Delay((uint32_t)(T*MU*K*(320-x)/20));
  //   }else if(350-y<0)
  //   {
  //     ChassisTransiation(Left,20,(uint32_t)(MU*K*(y-350)));
  //     HAL_Delay((uint32_t)(T*MU*K*(y-350)/20));
  //   }else if(350-y>0)
  //   {
  //     ChassisTransiation(Right,20,(uint32_t)(MU*K*(350-y)));
  //     HAL_Delay((uint32_t)(T*MU*K*(350-y)/20));
  //   }
  // }

  // Goods_Putdown(command[2]);
  // Lift_Back();
  CorrectError(90);
   ChassisTransiation(Back,40,(uint32_t)(REDZONE-zone[2]+75));
   ChassisRotate(ClockWise_Chassis,10,115);
   HAL_Delay(2000);
   CorrectError(0);
   ChassisTransiation(Back,50,240);
  ChassisTransiation(RightBack,20,8);
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
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

#ifdef  USE_FULL_ASSERT
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
