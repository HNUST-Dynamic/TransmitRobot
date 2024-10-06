#include "Vision.h"

//这个函数直接执行对标屏幕显示相关的最顶层任务



void Task_showing()
{
    
    OLED_Init();
    QR_recive(Orangepi);
    
    
    OLED_ShowString(44,90,command,24,1);
    HAL_UART_Transmit(&huart6,command,1,HAL_MAX_DELAY);
    USARTSend(&huart6,command,1,USART_TRANSFER_DMA);
    

}
