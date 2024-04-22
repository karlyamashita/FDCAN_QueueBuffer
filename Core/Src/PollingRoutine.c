/*
 * PollingRoutine.c
 *
 *  Created on: Oct 24, 2023
 *      Author: karl.yamashita
 *
 *
 *      Template for projects.
 *
 */


#include "main.h"

extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;

extern FDCAN_Struct fdcan1;
extern FDCAN_Struct fdcan2;

FDCAN_FilterTypeDef sFilterConfig;

// Called before main while loop
void PollingInit(void)
{
	// User can init CAN filters and interrupts

	// THIS IS AN EXAMPLE for hfdcan1 and FIFO_0 only! FIFO_1 needs to be initiated as well, along with hfdcan2
	/* Configure Rx filter */
	sFilterConfig.IdType = FDCAN_STANDARD_ID;
	sFilterConfig.FilterIndex = 0;
	sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	sFilterConfig.FilterID1 = 0x111;
	sFilterConfig.FilterID2 = 0x7FF; /* For acceptance, MessageID and FilterID1 must match exactly */
	HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig);

	/* Configure global filter to reject all non-matching frames */
	HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);

	/* Configure Rx FIFO 0 watermark to 2 */
	HAL_FDCAN_ConfigFifoWatermark(&hfdcan1, FDCAN_CFG_RX_FIFO0, 2);

	/* Activate Rx FIFO 0 watermark notification */
	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_WATERMARK, 0);


	HAL_FDCAN_Start(&hfdcan1);
	HAL_FDCAN_Start(&hfdcan2);
}

// main while loop
void PollingRoutine(void)
{
	FDCAN_CheckFifoRdy(&fdcan1);
	FDCAN_CheckFifoRdy(&fdcan2);

	FDCAN1_Parse(&fdcan1);
	FDCAN2_Parse(&fdcan2);

	FDCAN_Send(&fdcan1);
	FDCAN_Send(&fdcan2);
}

/*
 * Description: If fifoLocation is not zero, that means we have an interrupt from one of the FIFOx.
 * 				HAL_FDCAN_GetRxMessage will get that data from the FIFOx buffer and place into our queue buffer for processing in main while loop.
 */
void FDCAN_CheckFifoRdy(FDCAN_Struct *msg)
{
	CAN_RxData_t *ptr;

	if(msg->Rx.fifoLocation != 0)
	{
		ptr = &msg->Rx.Queue[msg->Rx.ptr.index_IN].canData;

		if(HAL_FDCAN_GetRxMessage(msg->fdcan, msg->Rx.fifoLocation, &ptr->rxHeader, ptr->data) == HAL_OK)
		{
			RingBuff_Ptr_Input(&msg->Rx.ptr, msg->Rx.queueSize);
			msg->Rx.fifoLocation = 0; // clear
		}
	}
}

/*
 * Description: Parse FDCAN1 data. User can manipulate data before passing to FDCAN2.
 */
void FDCAN1_Parse(FDCAN_Struct *msg)
{
	CAN_TxData_t txData = {0};

	if(FDCAN_MsgRdy(msg))
	{
		if(msg->Rx.msgToParse->rxHeader.Identifier == 0x100)
		{
			// user can manipulate data here
			msg->Rx.msgToParse->data[0] = 0x11; // change the first byte to 0x11;
		}
		else if(msg->Rx.msgToParse->rxHeader.Identifier == 0x200)
		{
			// user can manipulate data here
			msg->Rx.msgToParse->data[5] = 0x00; // clear the 5th byte data;
		}

		FDCAN_CopyRx_to_Tx(msg->Rx.msgToParse, &txData); // copy Rx data structure to Tx data structure
		FDCAN_AddToTx(&fdcan2, &txData); // add txData to Tx FDCAN2 queue
	}
}

/*
 * Description: Parse FDCAN2 data. User can manipulate data before passing to FDCAN1.
 */
void FDCAN2_Parse(FDCAN_Struct *msg)
{
	CAN_TxData_t txData = {0};

	if(FDCAN_MsgRdy(msg))
	{
		if(msg->Rx.msgToParse->rxHeader.Identifier == 0x400)
		{
			// user can manipulate data here
		}

		FDCAN_CopyRx_to_Tx(msg->Rx.msgToParse, &txData); // copy Rx data structure to Tx data structure
		FDCAN_AddToTx(&fdcan1, &txData); // add txData to Tx FDCAN1 queue
	}
}


