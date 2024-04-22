/*
 * CANFD_Handler.h
 *
 *  Created on: Apr 22, 2024
 *      Author: karl.yamashita
 */

#ifndef INC_CANFD_HANDLER_H_
#define INC_CANFD_HANDLER_H_


#define FDCAN_RX_QUEUE_SIZE 4
#define FDCAN_TX_QUEUE_SIZE 4
#define FDCAN_DATA_SIZE 64

typedef struct
{
	FDCAN_RxHeaderTypeDef rxHeader;
	uint8_t data[FDCAN_DATA_SIZE];
}CAN_RxData_t;

typedef struct
{
	FDCAN_TxHeaderTypeDef txHeader;
	uint8_t data[FDCAN_DATA_SIZE];
}CAN_TxData_t;

typedef struct
{
	FDCAN_HandleTypeDef *fdcan;
	struct
	{
		struct
		{
			CAN_RxData_t canData;
		}Queue[FDCAN_RX_QUEUE_SIZE];
		uint32_t queueSize;
		CAN_RxData_t *msgToParse;
		uint32_t fifoLocation;
		RING_BUFF_STRUCT ptr;
	}Rx;
	struct
	{
		struct
		{
			CAN_TxData_t canData;
		}Queue[FDCAN_TX_QUEUE_SIZE];
		uint32_t queueSize;
		RING_BUFF_STRUCT ptr;
	}Tx;
}FDCAN_Struct;

int FDCAN_MsgRdy(FDCAN_Struct *msg);
int FDCAN_AddToTx(FDCAN_Struct *msg, CAN_TxData_t *txMsg);
int FDCAN_Send(FDCAN_Struct *msg);
int FDCAN_CopyRx_to_Tx(CAN_RxData_t *rxMsg, CAN_TxData_t *txData);

#endif /* INC_CANFD_HANDLER_H_ */
