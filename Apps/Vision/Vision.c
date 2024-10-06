// #include "Vision.h"
// #include "usart.h"
// #include <math.h>
// #include "bsp_usart.h"

// // #define USART_REC_LEN 200
// // #define USART_EN_RX 1
// // #define RXBUFFERSIZE 16
// // #define RX_BUFFER 16  //缓存区大小
// // #define USARTCALLBACK2 
// // uint8_t RX_Buffer[RX_BUFFER];
// // uint8_t rxData;
// // char g_rx_buffer[];
// // uint8_t g_rx_buffer[RXBUFFERSIZE];
// // uint16_t g_usart_rx_sta = 0;
// // uint8_t g_rx_buffer[RXBUFFERSIZE];    /* HAL库使用的串口接收缓冲 */
// // uint8_t g_usart_rx_buf[USART_REC_LEN];

// uint8_t g_usart_rx_buf[USART_REC_LEN];
// uint16_t g_usart_rx_sta = 0;

// uint8_t g_rx_buffer[RXBUFFERSIZE];    /* HAL库使用的串口接收缓冲 */

// UART_HandleTypeDef g_uart1_handle;    /* UART句柄 */



// void Screen_Showing()
// {








// }


// //串口6的中断回调函数
// void CommandUSARTCallback(USARTInstance* USARTInstance, uint16_t Size) 
// {
//     if(huart6 ->Instance == &huart6)
//     {
//       if((g_usart_rx_sta & 0x8000) == 0)
//       {
//         if(g_usart_rx_sta & 0x4000)
//         {
//            if(g_rx_buffer[0] != 0x0a    )
//            {
//             g_usart_rx_sta = 0;
//            }
//            else 
//            {
//             g_usart_rx_sta |=0x8000;
//            }

//         }
//         else 
//         {
//            if(g_rx_buffer[0] == 0x0d)
//            {
//             g_usart_rx_sta |=0x4000;
//            }
//            else 
//            {
//             g_usart_rx_buf[g_usart_rx_sta &0x3FFF] = g_rx_buffer[0];
//             g_usart_rx_sta++;
//             if(g_usart_rx_sta >(USART_REC_LEN -1))
//             {
//                 g_usart_rx_sta = 0;
//             }
//            }

//         }
//       }
//     HAL_UART_Receive_IT(&huart6, (uint8_t*)g_rx_buffer,RXBUFFERSIZE);
//     }

// }
// void_CommandReceive()
// {
//    USART_Init_Config_s usart6_config;
//    usart6_config.recv_buff_size =  RX_BUFFER; 
//    usart6_config.usart_handle =  &huart6;
//    usart6_config.module_callback =  CommandUSARTCallback;   //这里创建一个给命令的串口实例

//    USARTInstance *Command_instance = USARTRegister(&usart6_config);
// }