#include "Cmd.h"
#include "bsp_usart.h"
#include "memory.h"
#include "usart.h"
#include "main.h"

#define RX_BUFFER 16  //缓存区大小
#define USARTCALLBACK


OrangepiInstance *Orangepi;
char command[16];


void QR_callback(USARTInstance* USARTInstance)
{
        
        if(0x31 == USARTInstance->recv_buff[0]&&0x32 == USARTInstance->recv_buff[9])
        {
                for(int i=0;i<9;i++)
                {
                        command[i] = USARTInstance->recv_buff[i+1];
                        HAL_UART_Transmit(&huart6,USARTInstance->recv_buff,1,HAL_MAX_DELAY);
                        HAL_UART_Transmit(&huart6,command,1,HAL_MAX_DELAY);
                }
        
                USARTSend(&huart6,&command,1,USART_TRANSFER_DMA);
        }
}
void QR_recive(OrangepiInstance *Orangepi)
{

   USART_Init_Config_s usart6_config;
   usart6_config.recv_buff_size =  RX_BUFFER; 
   usart6_config.usart_handle =  &huart6;
   usart6_config.module_callback =  QR_callback;   //这里创建一个给命令的串口实例

   USARTInstance *Command_instance = USARTRegister(&usart6_config);
    


}