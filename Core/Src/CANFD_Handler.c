/*
 * CANFD_Handler.c
 *
 *  Created on: Apr 22, 2024
 *      Author: karl.yamashita
 */

#include "main.h"

extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;

// Initialize variables
FDCAN_Struct fdcan1 =
{
	.fdcan = &hfdcan1,
	.Rx.queueSize = FDCAN_RX_QUEUE_SIZE,
	.Tx.queueSize = FDCAN_TX_QUEUE_SIZE
};

FDCAN_Struct fdcan2 =
{
	.fdcan = &hfdcan2,
	.Rx.queueSize = FDCAN_RX_QUEUE_SIZE,
	.Tx.queueSize = FDCAN_TX_QUEUE_SIZE
};

/*
 * Description: Check if message in queue. msgToParse points to current queue index.
 * Input: FDCAN_Struct data
 * Return: 1 = message available, 0 = no new message
 */
int FDCAN_MsgRdy(FDCAN_Struct *msg)
{
	if(msg->Rx.ptr.cnt_Handle)
	{
		msg->Rx.msgToParse = &msg->Rx.Queue[msg->Rx.ptr.index_OUT].canData;
		return 1;
	}

	return 0;
}

/*
 * Description: Add tx data to tx queue
 *
 */
int FDCAN_AddToTx(FDCAN_Struct *msg, CAN_TxData_t *txMsg)
{
	CAN_TxData_t *ptr = &msg->Tx.Queue[msg->Tx.ptr.index_IN].canData;

	memcpy(&ptr->txHeader, &txMsg->txHeader, sizeof(txMsg->txHeader));
	memcpy(&ptr->data, &txMsg->data, sizeof(txMsg->data));

	return 0;
}

/*
 * Description: Copy from RxHeader to TxHeader.
 */
int FDCAN_CopyRx_to_Tx(CAN_RxData_t *rxMsg, CAN_TxData_t *txData)
{
	txData->txHeader.IdType = rxMsg->rxHeader.IdType;
	txData->txHeader.Identifier = rxMsg->rxHeader.Identifier;
	txData->txHeader.DataLength = rxMsg->rxHeader.DataLength;
	// I've only show copying of 3 members.
	// User will need to add the rest of copying of the other data structures.

	memcpy(&txData->data, rxMsg->data, sizeof(rxMsg->data));

	return 0;
}

/*
 * Description: Check queue for message to send. ptr points to current queue index.
 * Input: FDCAN_Struct data
 * Return: 0 = Message added to Fifo, 1 = CAN Busy, -1 = queue was empty
 */
int FDCAN_Send(FDCAN_Struct *msg)
{
	CAN_TxData_t ptr;

	if(msg->Tx.ptr.cnt_Handle)
	{
		ptr = msg->Tx.Queue[msg->Tx.ptr.index_OUT].canData;

		if(HAL_FDCAN_AddMessageToTxFifoQ(msg->fdcan, &ptr.txHeader, ptr.data) == HAL_OK)
		{
			RingBuff_Ptr_Output(&msg->Tx.ptr, msg->Tx.queueSize); // Increment queue pointer only if HAL_OK, else try to add to Fifo in next loop.
			return 0;
		}
		return 1;
	}

	return -1;
}

// FIFO 0
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
	if(hfdcan == fdcan1.fdcan)
	{
		if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_WATERMARK) != RESET)
		{
			fdcan1.Rx.fifoLocation = FDCAN_RX_FIFO0;
		}
	}
	else if(hfdcan == fdcan2.fdcan)
	{
		if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_WATERMARK) != RESET)
		{
			fdcan2.Rx.fifoLocation = FDCAN_RX_FIFO0;
		}
	}
}

// FIFO 1
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
	if(hfdcan == fdcan1.fdcan)
	{
		if((RxFifo1ITs & FDCAN_IT_RX_FIFO1_WATERMARK) != RESET)
		{
			fdcan1.Rx.fifoLocation = FDCAN_RX_FIFO1;
		}
	}
	else if(hfdcan == fdcan2.fdcan)
	{
		if((RxFifo1ITs & FDCAN_IT_RX_FIFO1_WATERMARK) != RESET)
		{
			fdcan2.Rx.fifoLocation = FDCAN_RX_FIFO1;
		}
	}
}
