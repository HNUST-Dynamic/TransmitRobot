#include "Cmd.h"
#include "bsp_usart.h"
#include "memory.h"
#include "usart.h"
#include "main.h"

#define RX_BUFFER 16  //缓存区大小
#define USARTCALLBACK
#define DISTANCE 10

OrangepiInstance *Orangepi;
volatile uint8_t command[8];
volatile uint8_t rangging[6];
uint8_t r_command[8];

volatile uint8_t wuliao[24];
int now_x,now_y,rang,now_wuliao=6;
int last_x=0,last_y=0,last_wuliao=6;
int difference_x,difference_y;
int error_x ,error_y;
int x_int,y_int;
int last_x_int,last_y_int;
int d;
char temp[6];
uint8_t element;


int num=0;



void Cmd_Callback(USARTInstance* USARTInstance)
{       
        if((0x01 == USARTInstance->recv_buff[0]) && (0x03 == USARTInstance->recv_buff[8]))
        {
                for(int i=0;i<7;i++)
                {
                        command[i] = USARTInstance->recv_buff[i+1];
                }
                command[7]='\0';
                for(int i=0;i<24;i++)
                {
                        USARTInstance->recv_buff[i]=0;
                }
        }
        
        if(0x28==USARTInstance->recv_buff[2])
        {
          switch (USARTInstance->recv_buff[0])
          {
                case 0x30 : 
                wuliao[0]=0x30;
                break;
                case 0x31 : 
                wuliao[0]=0x31;
                break;
                case 0x32 : 
                wuliao[0]=0x32;
                break;
                case 0x33 : 
                wuliao[0]=0x33;
                break;
                case 0x34 : 
                wuliao[0]=0x35;
                break;
                case 0x35 : 
                wuliao[0]=0x34;
                break;
                default:
                break;
          }

          for(int i=1;i<16;i++)
          {
                wuliao[i] = USARTInstance->recv_buff[i];
          }
          element=wuliao[0]; 


          // 将十六进制数据转换为字符串
          char str_data[12];
          float x, y;
          for (int i = 0; i < 12; i++) 
          {
                str_data[i] = (char)wuliao[i+3];
          }
          
          int i = 0, j = 0;
        
        //   // 跳过第一个括号
        //   while (str_data[i] == '(' && str_data[i] != '\0') i++;
        //   i++;

          // 读取第一个浮点数
          while (str_data[i] != ',' && str_data[i] != '\0') {
                  temp[j++] = str_data[i++];
          }
          temp[j] = '\0';
          x = atof(temp);

          // 重置 temp 索引
          j = 0;
          i++;

          // 读取第二个浮点数
          while (str_data[i] != ')' && str_data[i] != '\0') {
                  temp[j++] = str_data[i++];
          }
          temp[j] = '\0';
          y = atof(temp);

          last_x_int = x_int;
          last_y_int = y_int;

          // 将浮点数转换为整数
          x_int = (int)x;
          y_int = (int)y;
         __HAL_UART_DISABLE_IT(&huart6, UART_IT_IDLE);

        }
        if((0x02 == USARTInstance->recv_buff[0]) )
        {
                for(int i=0;i<6;i++)
                {
                        rangging[i] = USARTInstance->recv_buff[i+1];
                }
                
                // 将十六进制数据转换为字符串
                char str_data[8];
                for (int i = 0; i < 6; i++) 
                {
                        str_data[i] = (char)rangging[i];
                }

                // 提取x和y的字符串值
                float x;
                //sscanf(str_data, "%f", &x);
                x=atof(str_data);
                // 将浮点数转换为整数
                d = (int)x;
                
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
        for(int i=0;i<7;i++)
        {
                if(num!=3&&(wuliao[0]-2==command[num]))
                {
                        num++;
                        return true;                                         
                }
        }
        return false;
        
}
bool IsStable()
{
        if(0x2C==wuliao[1]&&0x28==wuliao[2])
        {
              last_wuliao=now_wuliao;
              now_wuliao=wuliao[0];
              if(now_wuliao==last_wuliao)
              {
                        difference_x=x_int-last_x_int;
                        difference_y=y_int-last_y_int;
                                                
                        if(difference_x*difference_x+difference_y*difference_y<=100000)
                        {
                                return true;
                        }
               }
        }
        HAL_Delay(500);
        __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
        return false;
}
bool IsAccurate()
{       
        now_x=(int)(wuliao[2]-0x30)*10*10+(int)(wuliao[3]-0x30)*10+(int)(wuliao[4]-0x30);//+wuliao[6]/16;
        now_y=(int)(wuliao[13]-0x30)*10*10+(int)(wuliao[14]-0x30)*10+(int)(wuliao[15]-0x30);//+wuliao[12]/16;
        if((0x1E<now_x&&now_x<0x154)&&(0x1E<now_y&&now_y<0x154))
        {
                return true;
        }
}
int x_error()
{
        now_x=(int)(wuliao[2]-0x30)*10*10+(int)(wuliao[3]-0x30)*10+(int)(wuliao[4]-0x30);//+wuliao[6]/16;
        return now_x;

}
int y_error()
{
        now_y=(int)(wuliao[13]-0x30)*10*10+(int)(wuliao[14]-0x30)*10+(int)(wuliao[15]-0x30);//+wuliao[12]/16;
        return now_y;
}
int rang_error()
{
        rang=(int)(rangging[0]-0x30)*10+(int)(rangging[1]-0x30);
        return rang;
}
