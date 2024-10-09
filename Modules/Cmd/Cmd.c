#include "Cmd.h"
#include "bsp_usart.h"
#include "memory.h"
#include "usart.h"
#include "main.h"

#define RX_BUFFER 16  //缓存区大小
#define USARTCALLBACK
#define DISTANCE 10

OrangepiInstance *Orangepi;
char command[8];

uint8_t wuliao[16];
uint8_t now_x,now_y,now_wuliao;
uint8_t last_x=0,last_y=0,last_wuliao=6;
uint8_t difference_x,difference_y;
uint8_t element;


int num=0;


void Cmd_Callback(USARTInstance* USARTInstance)
{       
        if((0x02 == USARTInstance->recv_buff[0]) && (0x03 == USARTInstance->recv_buff[8]))
        {
                for(int i=0;i<7;i++)
                {
                        command[i] = USARTInstance->recv_buff[i+1];
                }
                        command[7]='\0';
        }
        if(0x0A==USARTInstance->recv_buff[1]&&0x0A==USARTInstance->recv_buff[15])
        {
                for(int i=0;i<16;i++)
                {
                        wuliao[i] = USARTInstance->recv_buff[i];
                }
                        element=wuliao[0];
        }

}
void CmdUart_Init()
{
        
        USART_Init_Config_s usart6_config;
        usart6_config.recv_buff_size =  RX_BUFFER; 
        usart6_config.usart_handle =  &huart6;
        usart6_config.module_callback =  Cmd_Callback;   //这里创建一个给命令的串口实例

        USARTInstance *Command_instance = USARTRegister(&usart6_config);
 
}
bool IsMatch()
{
        if(0x0A==wuliao[1]&&0x0A==wuliao[15])
        {
                for(int i=0x30;i<0x36;i++)
                {
                        if(i==wuliao[0])
                        {
                                if((wuliao[0]+1==command[num++])&&num!=3)
                                {
                                      return true;                                         
                                }
                        }
                }
        }
        return false;
}
bool IsStable()
{
        if(0x2E==wuliao[6]&&0x2C==wuliao[8]&&0x2E==wuliao[12]&&0x29==wuliao[14])
        {
              now_wuliao=wuliao[0];
              if(now_wuliao==last_wuliao)
              {
               now_x=wuliao[3]*16*16+wuliao[4]*16+wuliao[5];//+wuliao[6]/16;
               now_y=wuliao[9]*16*16+wuliao[10]*16+wuliao[11];//+wuliao[12]/16;
                       
               difference_x=now_x-last_x;
               difference_y=now_y-last_y;
                       
               last_x=now_x;
               last_y=now_y;
               last_wuliao=now_wuliao;m 
               if(difference_x*difference_x+difference_y*difference_y<=1000000)
               {
                       return true;
               }
              }
              

        }
        return false;
}

