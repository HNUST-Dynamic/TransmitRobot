#include "Cmd.h"
#include "bsp_usart.h"
#include "memory.h"
#include "usart.h"
#include "main.h"
#include <string.h>

#define RX_BUFFER 16            // 缓存区大小
#define DISTANCE 10             // 二维码测距时的目标距离

OrangepiInstance *Orangepi;     // 香橙派实例
volatile uint8_t command[8];    // 扫描二维码接收到的命令数组
volatile uint8_t rangging[6];   // 扫描二维码接收到的距离字符串所对应的十六进制
volatile uint8_t wuliao[24];    
char now_wuliao = '6';          // 识别物料时记录物料
char last_wuliao = '6';
int difference_x, difference_y; // x和y的坐标差值
int x_int, y_int;               // 坐标转码后的值
int last_x_int, last_y_int;     // 上次坐标转码后的值
int d;                          // 二维码测距的转码值
char temp[6];                   // 转码操作时的临时数组
uint8_t element;                // 当前物料值
int num = 0;                    // 判断匹配函数中的迭代变量
char line[16];                  // 与上位机通讯时每帧中的每一行信息
const char *delimiter = '\n';   // 与上位机通讯时一帧数据中每一行的分隔符
volatile char input_copy[256];  // 从中断回调函数中拷贝出来的缓存区内容

void Cmd_Callback(USARTInstance *USARTInstance)
{
        // 二维码内容信息
        if ((0x01 == USARTInstance->recv_buff[0]) && (0x03 == USARTInstance->recv_buff[8]))
        {

                for (int i = 0; i < 7; i++)
                {
                        command[i] = USARTInstance->recv_buff[i + 1];
                }
                command[7] = '\0';
                for (int i = 0; i < 24; i++)
                {
                        USARTInstance->recv_buff[i] = 0;
                }
        }
        // 返回的物料或者圆环信息
        if (0x28 == USARTInstance->recv_buff[2])
        {
                strncpy(input_copy, USARTInstance->recv_buff, sizeof(input_copy) - 1);
                input_copy[sizeof(input_copy) - 1] = '\0';
                switch (USARTInstance->recv_buff[0])
                {
                case 0x30:
                        wuliao[0] = 0x30;
                        break;
                case 0x31:
                        wuliao[0] = 0x31;
                        break;
                case 0x32:
                        wuliao[0] = 0x32;
                        break;
                case 0x33:
                        wuliao[0] = 0x33;
                        break;
                case 0x34:
                        wuliao[0] = 0x35;
                        break;
                case 0x35:
                        wuliao[0] = 0x34;
                        break;
                default:
                        break;
                }

                for (int i = 1; i < 16; i++)
                {
                        wuliao[i] = USARTInstance->recv_buff[i];
                }
                element = wuliao[0];

                __HAL_UART_DISABLE_IT(&huart6, UART_IT_IDLE);
                __HAL_UART_DISABLE_IT(&huart6, UART_IT_RXNE);
        }
        // 二维码距离信息
        if ((0x02 == USARTInstance->recv_buff[0]))
        {
                strncpy(input_copy, USARTInstance->recv_buff, sizeof(input_copy) - 1);
                input_copy[sizeof(input_copy) - 1] = '\0';

                for (int i = 0; i < 6; i++)
                {
                        rangging[i] = USARTInstance->recv_buff[i + 1];
                }

                // 将十六进制数据转换为字符串
                char str_data[8];
                for (int i = 0; i < 6; i++)
                {
                        str_data[i] = (char)rangging[i];
                }

                // 提取x和y的字符串值
                float x;
                // sscanf(str_data, "%f", &x);
                x = atof(str_data);
                // 将浮点数转换为整数
                d = (int)x;
        }
}
void CmdUart_Init()
{

        USART_Init_Config_s usart6_config;
        usart6_config.recv_buff_size = RX_BUFFER;
        usart6_config.usart_handle = &huart6;
        usart6_config.module_callback = Cmd_Callback; // 这里创建一个给命令的串口实例

        USARTInstance *Command_instance = USARTRegister(&usart6_config);
}
bool IsMatch(char current_ring)
{
        char *token = strtok(input_copy, "\n");
        while (token != NULL)
        {
                strncpy(line, token, sizeof(line) - 1);
                line[sizeof(line) - 1] = '\0';
                switch (line[0])
                {
                case 0x30:
                        line[0] = 0x30;
                        break;
                case 0x31:
                        line[0] = 0x31;
                        break;
                case 0x32:
                        line[0] = 0x32;
                        break;
                case 0x33:
                        line[0] = 0x33;
                        break;
                case 0x34:
                        line[0] = 0x35;
                        break;
                case 0x35:
                        line[0] = 0x34;
                        break;
                default:
                        break;
                }

                if (line[0] == current_ring)
                {
                        for (int i = 0; i < 7; i++)
                        {
                                if (num != 3 && ((uint8_t)(current_ring - 2)  == command[num]))
                                {
                                        return true;
                                }
                        }
                        return false;
                }
                // 获取下一行
                token = strtok(NULL, "\n");
        }
}
bool IsStable(char current_ring)
{
        char *token = strtok(input_copy, "\n");
        // memset(input_copy,0,sizeof(input_copy));

        while (token != NULL)
        {
                strncpy(line, token, sizeof(line) - 1);
                line[sizeof(line) - 1] = '\0';
                switch (line[0])
                {
                case 0x30:
                        line[0] = 0x30;
                        break;
                case 0x31:
                        line[0] = 0x31;
                        break;
                case 0x32:
                        line[0] = 0x32;
                        break;
                case 0x33:
                        line[0] = 0x33;
                        break;
                case 0x34:
                        line[0] = 0x35;
                        break;
                case 0x35:
                        line[0] = 0x34;
                        break;
                default:
                        break;
                }

                if (line[0] == current_ring)
                {
                        // 将十六进制数据转换为字符串
                        char str_data[12];
                        float x, y;
                        for (int i = 0; i < 12; i++)
                        {
                                str_data[i] = (char)line[i + 3];
                        }

                        int i = 0, j = 0;

                        //   // 跳过第一个括号
                        //   while (str_data[i] == '(' && str_data[i] != '\0') i++;
                        //   i++;

                        // 读取第一个浮点数
                        while (str_data[i] != ',' && str_data[i] != '\0')
                        {
                                temp[j++] = str_data[i++];
                        }
                        temp[j] = '\0';
                        x = atof(temp);

                        // 重置 temp 索引
                        j = 0;
                        i++;

                        // 读取第二个浮点数
                        while (str_data[i] != ')' && str_data[i] != '\0')
                        {
                                temp[j++] = str_data[i++];
                        }
                        temp[j] = '\0';
                        y = atof(temp);


                        // 将浮点数转换为整数
                        x_int = (int)x;
                        y_int = (int)y;

                        last_wuliao = now_wuliao;
                        now_wuliao = line[0];
                        if (now_wuliao == last_wuliao)
                        {
                                difference_x = x_int - last_x_int;
                                difference_y = y_int - last_y_int;

                                if (difference_x * difference_x + difference_y * difference_y <= 500)
                                {
                                        __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);
                                        __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
                                        return true;
                                }
                        }
                        last_x_int = x_int;
                        last_y_int = y_int;
                          memset(input_copy, 0, sizeof(input_copy));

                        // HAL_Delay(200);
                        __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);
                        __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
                        return false;
                }
                // 获取下一行
                token = strtok(NULL, "\n");
        }
}
bool Ring_IsStable(char current_ring)
{
        char *token = strtok(input_copy, "\n");
        while (token != NULL)
        {
                strncpy(line, token, sizeof(line) - 1);
                line[sizeof(line) - 1] = '\0';
                switch (line[0])
                {
                case 0x30:
                        line[0] = 0x30;
                        break;
                case 0x31:
                        line[0] = 0x31;
                        break;
                case 0x32:
                        line[0] = 0x32;
                        break;
                case 0x33:
                        line[0] = 0x33;
                        break;
                case 0x34:
                        line[0] = 0x35;
                        break;
                case 0x35:
                        line[0] = 0x34;
                        break;
                default:
                        break;
                }

                if (line[0] == current_ring)
                {
                        // 将十六进制数据转换为字符串
                        char str_data[12];
                        float x, y;
                        for (int i = 0; i < 12; i++)
                        {
                                str_data[i] = (char)line[i + 3];
                        }

                        int i = 0, j = 0;

                        //   // 跳过第一个括号
                        //   while (str_data[i] == '(' && str_data[i] != '\0') i++;
                        //   i++;

                        // 读取第一个浮点数
                        while (str_data[i] != ',' && str_data[i] != '\0')
                        {
                                temp[j++] = str_data[i++];
                        }
                        temp[j] = '\0';
                        x = atof(temp);

                        // 重置 temp 索引
                        j = 0;
                        i++;

                        // 读取第二个浮点数
                        while (str_data[i] != ')' && str_data[i] != '\0')
                        {
                                temp[j++] = str_data[i++];
                        }
                        temp[j] = '\0';
                        y = atof(temp);


                        // 将浮点数转换为整数
                        x_int = (int)x;
                        y_int = (int)y;

                        difference_x = x_int - last_x_int;
                        difference_y = y_int - last_y_int;

                        if (difference_x * difference_x + difference_y * difference_y <= 50)
                        {
                                __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
                                __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);
                                return true;
                        }
                        // HAL_Delay(200);
                        last_x_int = x_int;
                        last_y_int = y_int;
                          memset(input_copy, 0, sizeof(input_copy));

                        __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
                        __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);
                        return false;
                }
                // 获取下一行
                token = strtok(NULL, "\n");
        }
}
