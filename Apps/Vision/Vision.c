#include "Vision.h"

//这个函数直接执行对标屏幕显示相关的最顶层任务

void Vision_Init()//必须放在while函数外面
{
    OLED_Init();
    OLED_Fill(0,0,256,128,0x00);
    CmdUart_Init();
    
}

void OLED_Show()//要放在while函数才能执行
{
    OLED_ShowString(22,45,"command",32,0);
    //OLED_ShowString(22,45,command,32,0);
}



