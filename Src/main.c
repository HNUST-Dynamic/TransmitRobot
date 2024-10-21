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
#define REDZONE           0
#define GREENZONE         130
#define BLUEZONE          260
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  uint8_t zone[3];
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
  ElevatorMotor_Init();
 
  // IMUInit();
  // strcpy(command, "123+321");
  // while(IsStable())
  // {
  //   HAL_UART_Transmit(&huart6,(uint8_t *)"1",1,HAL_MAX_DELAY);
  // }
  
  /*出来*/
  ElevatorMotor_Init();
  ChassisTransiation(Left,20,180);
   HAL_Delay(1000);
  ChassisTransiation(Forward,20,600);
  Lift_StartFirst();
  Lift_updown_control(down,1000,200000);
  HAL_Delay(6000);

  /*问香橙派QRCode 阻塞*/
  while(command[7]!='\0'){}
  Lift_updown_control(up,1000,200000);
  HAL_Delay(6000);

  /*OLED展示顺序*/
  OLED_Show();

  #if (RANGING==1)
  while(timeout_count<10)
  {
    /*与香橙派问答测距*/
    timeout_count++;
  }
  timeout_count = 0;
  #endif


  /*出发去转盘*/
   ChassisTransiation(Forward,20,700);
   HAL_Delay(3300);
   /*靠近转盘*/

   ChassisTransiation(Right,20,83);
   HAL_Delay(1500);
  /*在 物料稳定 并且 与当前要抓的匹配 时 抓取x3*/
   for(int i = 0;i < 3;i++)
   {
       while(!(IsStable() && IsMatch())){}
      Lift_Catch(element);
      HAL_Delay(1000);
      Lift_Back();
   }
     /*离开转盘*/
   ChassisTransiation(Left,20,63);
   HAL_Delay(1500);

  /*出发去暂存区*/
   ChassisTransiation(Forward,20,360);
   HAL_Delay(3000);
   ChassisRotate(CounterClockWise_Chassis,10,90);
   HAL_Delay(1000);
   ChassisTransiation(Forward,20,630);
   HAL_Delay(4000);

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
  /*靠近暂存区 第一个颜色区*/
   ChassisTransiation(Right,20,110);
   HAL_Delay(1200);
   ChassisTransiation(Forward,20,zone[0]);
   HAL_Delay(1200);
  while(!(IsStable())){};
  Goods_Putdown(command[0]);
  /*第二个颜色区*/
  //  ChassisTransiation(Forward,20,125);
  //  HAL_Delay(1200);
  if((zone[1]-zone[0])>0)
  {
    ChassisTransiation(Forward,20,(zone[1]-zone[0]));   
    HAL_Delay(1200);
  }else{
    ChassisTransiation(Back,20,(zone[0]-zone[1]));
     HAL_Delay(1200);
  }
    while(!(IsStable())){};
  Goods_Putdown(command[1]);
  Lift_Back();
  /*第三个颜色区*/
  //  ChassisTransiation(Forward,20,125);
  //  HAL_Delay(1200);
    if((zone[2]-zone[1])>0)
  {
    ChassisTransiation(Forward,20,(zone[2]-zone[1]));
     HAL_Delay(1200);
  }else{
    ChassisTransiation(Back,20,(zone[1]-zone[2]));
    HAL_Delay(1200);
  }
  while(!(IsStable())){};
  Goods_Putdown(command[2]);
  Lift_Back();



  // /*靠近暂存区 第一个颜色区*/
  //  //hassisTransiation(Right,20,110);
  //  //HAL_Delay(1200);
  //  ChassisTransiation(Forward,20,zone[0]);
  //  HAL_Delay(1200);
  // while(!(IsStable())){}
  // Lift_Catch_two(element);
  // HAL_Delay(1000);
  // Lift_Back();
  // /*第二个颜色区*/
  // //  ChassisTransiation(Forward,20,125);
  // //  HAL_Delay(1200);
  // if((zone[1]-zone[0])>0)
  // {
  //   ChassisTransiation(Forward,20,(zone[1]-zone[0]));   
  //   HAL_Delay(1200);
  // }else{
  //   ChassisTransiation(Back,20,(zone[0]-zone[1]));
  //    HAL_Delay(1200);
  // }
  // while(!(IsStable())){}
  // Lift_Catch_two(element);
  // HAL_Delay(1000);
  // Lift_Back();
  // /*第三个颜色区*/
  // //  ChassisTransiation(Forward,20,125);
  // //  HAL_Delay(1200);
  //   if((zone[2]-zone[1])>0)
  // {
  //   ChassisTransiation(Forward,20,(zone[2]-zone[1]));
  //    HAL_Delay(1200);
  // }else{
  //   ChassisTransiation(Back,20,(zone[1]-zone[2]));
  //   HAL_Delay(1200);
  // }
  // while(!(IsStable())){}
  // Lift_Catch_two(element);
  // HAL_Delay(1000);
  // Lift_Back();

   
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
