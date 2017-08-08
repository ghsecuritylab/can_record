#ifndef __COMMON_H__
#define __COMMON_H__
#include <rthw.h>
#include <rtthread.h>
#include "stm32f4xx_can.h"

// �ڴ�صĿ��С
#define	MEMPOLL_SIZE		2048

// ����״̬��
typedef enum 
{
	INIT,
	STOP,
	RUN,
	RTC_ERROR,
	SD_ERROR,
	INIT_ERROR
}status_type_t;

// ȫ�ֽӿ�
typedef struct 
{
	status_type_t status;
	struct rt_mempool mempool;
}global_t;


extern global_t global;


extern int rt_can1_init(void);
extern int rt_can2_init(void);
// ��ʼ���ڴ��
extern rt_err_t mempool_init(void);

extern void can_init(CAN_TypeDef* CANx, unsigned int bps);
extern void can_filter_init(unsigned int num);
extern void can_nvic_config(CAN_TypeDef* CANx,FunctionalState NewState);
extern void can_send_test(CAN_TypeDef* CANx,char data);
#endif
