#include "Cmd.h"
#include "bsp_usart.h"
#include "memory.h"
#include "usart.h"
#include "main.h"

#define RX_BUFFER 16  //缓存区大小
#define USARTCALLBACK


OrangepiInstance *Orangepi;
char command[8];

void QR_callback(USARTInstance* USARTInstance)
{
        
        if((0x02 == USARTInstance->recv_buff[0]) && (0x03 == USARTInstance->recv_buff[8]))
        {
                for(int i=0;i<7;i++)
                {
                        command[i] = USARTInstance->recv_buff[i+1];
                }
                        command[7]='\0';
        }

        USARTSend(USARTInstance,command,7,USART_TRANSFER_DMA);

}
void QR_recive()
{

        USART_Init_Config_s usart6_config;
        usart6_config.recv_buff_size =  RX_BUFFER; 
        usart6_config.usart_handle =  &huart6;
        usart6_config.module_callback =  QR_callback;   //这里创建一个给命令的串口实例

        USARTInstance *Command_instance = USARTRegister(&usart6_config);
 
}