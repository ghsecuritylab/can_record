#include "common.h"

global_t global;


rt_err_t mempool_init()
{
	return rt_mp_init(&global.mempool, "mp1",(void *)0x10000000, 0x10000, MEMPOLL_SIZE);
}

void can_init(CAN_TypeDef* CANx, unsigned int bps)
{	
	CAN_InitTypeDef CAN_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	
	if(CANx == CAN1)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
		NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}else
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
		NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
	}
	CAN_ITConfig(CANx, CAN_IT_FMP0, DISABLE);
	/*
		TTCM = time triggered communication mode
		ABOM = automatic bus-off management 
		AWUM = automatic wake-up mode
		NART = no automatic retransmission
		RFLM = receive FIFO locked mode 
		TXFP = transmit FIFO priority		
	*/
	CAN_InitStructure.CAN_TTCM = DISABLE;			/* 禁止时间触发模式（不生成时间戳), T  */
	CAN_InitStructure.CAN_ABOM = DISABLE;			/* 禁止自动总线关闭管理 */
	CAN_InitStructure.CAN_AWUM = DISABLE;			/* 禁止自动唤醒模式 */
	CAN_InitStructure.CAN_NART = DISABLE;			/* 禁止仲裁丢失或出错后的自动重传功能 */
	CAN_InitStructure.CAN_RFLM = DISABLE;			/* 禁止接收FIFO加锁模式 */
	CAN_InitStructure.CAN_TXFP = DISABLE;			/* 禁止传输FIFO优先级 */
	CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;;	/* 设置CAN为回环模式 */
	
	/* 
		CAN 波特率 = RCC_APB1Periph_CAN1 / Prescaler / (SJW + BS1 + BS2);
		
		SJW = synchronisation_jump_width 
		BS = bit_segment
		
		本例中，设置CAN波特率为1Mbps		
		CAN 波特率 = 420000000 / 2 / (1 + 12 + 8) / = 1 Mbps		
	*/
				
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
	// 2Mbps
	switch(bps)
	{
		case 500000:
			CAN_InitStructure.CAN_Prescaler = 4;
			break;
		case 250000:
			CAN_InitStructure.CAN_Prescaler = 8;
			break;
		case 100000:
			CAN_InitStructure.CAN_Prescaler = 20;
			break;
		default:
			CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
			CAN_InitStructure.CAN_BS1 = CAN_BS1_12tq;
			CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
			CAN_InitStructure.CAN_Prescaler = 2;
			break;
	}
	CAN_Init(CANx, &CAN_InitStructure);
	CAN_SlaveStartBank(14);
}
void can_filter_init(unsigned int num)
{
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	CAN_FilterInitStructure.CAN_FilterNumber = num;	/* 筛选器序号，0-27，共28个筛选器 */
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;		/* 筛选器模式，设置ID掩码模式 */
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;	/* 32位筛选 */
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;					/* 掩码后ID的高16bit */
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;					/* 掩码后ID的低16bit */
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;				/* ID掩码值高16bit */
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;				/* ID掩码值低16bit */
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;		/* 筛选器绑定FIFO 0 */
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;				/* 使能筛选器 */
	CAN_FilterInit(&CAN_FilterInitStructure);
}


void can_nvic_config(CAN_TypeDef* CANx,FunctionalState NewState)
{
	/* CAN FIFO0 消息接收中断使能 */ 
	CAN_ITConfig(CANx, CAN_IT_FMP0, NewState);	
}

void can_send_test(CAN_TypeDef* CANx,char data)
{
	static CanTxMsg TxMessage;
	TxMessage.StdId = 0x321;
	TxMessage.ExtId = 0x01;
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.IDE = CAN_ID_STD;
	TxMessage.DLC = 1;
	TxMessage.Data[0] = data;
	CAN_Transmit(CANx, &TxMessage);	
}
