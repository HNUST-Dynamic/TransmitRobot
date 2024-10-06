#include "Vision.h"

//这个函数直接执行对标屏幕显示相关的最顶层任务

void Task_showing()
{
    OLED_Init();
    OLED_Fill(0,0,256,128,0x00);
    QR_recive();
    
    
    OLED_ShowString(44,90,"command",24,1);
    // HAL_UART_Transmit(&huart6,&buff,1,HAL_MAX_DELAY);

    // HAL_Delay(100);
    

}
