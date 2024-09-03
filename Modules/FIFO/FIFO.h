#ifndef __FIFO_H
#define __FIFO_H

#include <stdint.h>
#include "stm32f407xx.h"

#define 	FIFO_SIZE   128

typedef struct {
	uint16_t buffer[FIFO_SIZE];
	__IO uint8_t ptrWrite;
	__IO uint8_t ptrRead;
}FIFO_t;

extern __IO FIFO_t rxFIFO;

/**
	* @brief   初始化队列
	* @param   无
	* @retval  无
	*/
void fifo_initQueue(void);

/**
	* @brief   入队
	* @param   无
	* @retval  无
	*/
void fifo_enQueue(uint16_t data);

/**
	* @brief   出队
	* @param   无
	* @retval  无
	*/
uint16_t fifo_deQueue(void);

/**
	* @brief   判断空队列
	* @param   无
	* @retval  无
	*/
bool fifo_isEmpty(void);

/**
	* @brief   计算队列长度
	* @param   无
	* @retval  无
	*/
uint16_t fifo_queueLength(void);

#endif
