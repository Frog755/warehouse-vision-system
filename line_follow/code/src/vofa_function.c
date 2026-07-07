#include "vofa_function.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "base_transfer.h"

/* 外部声明：来自 usart.cpp */
extern uint8_t vofaRxBufferIndex;
extern void uartSendByte(uint8_t c);
extern void uartSendData(uint8_t* Array, uint8_t SIZE);

/* VOFA 结构体 */
vofaJustFloatFrame JustFloat_Data;
vofaCommand        vofaCommandData;

/* 命令回调指针 */
static VofaCmdCallback s_cmdCallback = NULL;

/* ========== 发送函数 ========== */

void vofaSendJustFloat(vofaJustFloatFrame* vofaJFFrame)
{
	uint8_t i;
	uint8_t u8Array[4];
	for (i = 0; i < CH_COUNT; i++)
	{
		float2uint8Array(u8Array, &vofaJFFrame->fdata[i]);
		uartSendData(u8Array, sizeof(u8Array));
	}
	uartSendData(vofaJFFrame->frametail, FRAME_TAIL_SIZE);
}

void vofaSendFirewater(const float* fdata, const uint32_t ulSize)
{
	uint32_t i;
	char buffer[64];
	int len;

	for (i = 0; i < ulSize - 1; i++)
	{
		len = snprintf(buffer, sizeof(buffer), "%.6f,", *(fdata + i));
		uartSendData((uint8_t*)buffer, len);
	}
	len = snprintf(buffer, sizeof(buffer), "%.6f\n", *(fdata + i));
	uartSendData((uint8_t*)buffer, len);
}

void vofaSendRawdata(uint8_t* pData, const uint32_t ulSize)
{
	uartSendData(pData, ulSize);
}

/* ========== 初始化 ========== */

void vofaJustFloatInit(void)
{
	vofaCommandData.cmdID          = INVALID;
	vofaCommandData.cmdType        = INVALID;
	vofaCommandData.completionFlag = 0;
	JustFloat_Data.frametail[0]    = 0x00;
	JustFloat_Data.frametail[1]    = 0x00;
	JustFloat_Data.frametail[2]    = 0x80;
	JustFloat_Data.frametail[3]    = 0x7f;
}

/* ========== 接收与解析（状态机版） ========== */

void uartCMDRecv(uint8_t byte_data)
{
	static uint8_t rxBuf[32];   // 接收缓冲区，比 CMD_FRAME_SIZE 大
	static uint8_t idx = 0;
	static uint8_t synced = 0;  // 是否已同步到帧头

	// 跳过换行符和回车符（VOFA 可能自动添加）
	if (byte_data == '\n' || byte_data == '\r')
	{
		return;
	}

	// 找帧头 '@'
	if (!synced)
	{
		if (byte_data == '@')
		{
			rxBuf[0] = '@';
			idx = 1;
			synced = 1;
		}
		// 不是 '@' 就丢弃
		return;
	}

	// 已同步，继续收数据
	if (idx < sizeof(rxBuf) - 1)
	{
		rxBuf[idx] = byte_data;
		idx++;
	}
	else
	{
		// 缓冲区溢出，重置
		synced = 0;
		idx = 0;
		return;
	}

	// 检查是否收到帧尾 '!#'
	if (idx >= 2 && rxBuf[idx - 2] == '!' && rxBuf[idx - 1] == '#')
	{
		// 收到完整帧，复制到正式结构体
		uint8_t frameLen = idx;
		if (frameLen <= CMD_FRAME_SIZE)
		{
			memcpy(vofaCommandData.uartRxPacket, rxBuf, frameLen);
			vofaCommandData.completionFlag = 1;
		}
		// 重置准备下一帧
		synced = 0;
		idx = 0;
	}
}

void vofaCommandParse(void)
{
	// 基本格式检查：@ X X = ... ! #
	if (vofaCommandData.uartRxPacket[0] != '@' ||
	    vofaCommandData.uartRxPacket[3] != '=')
	{
		goto parse_fail;
	}

	// 找帧尾 '!' 的位置
	uint8_t* pEnd = memchr(vofaCommandData.uartRxPacket, '!', CMD_FRAME_SIZE);
	if (pEnd == NULL || *(pEnd + 1) != '#')
	{
		goto parse_fail;
	}

	// 解析命令类型（索引1）
	switch (vofaCommandData.uartRxPacket[1])
	{
		case 'S': vofaCommandData.cmdType = Speed; break;
		case 'P': vofaCommandData.cmdType = Position; break;
		default:  vofaCommandData.cmdType = INVALID; goto parse_fail;
	}

	// 解析命令ID（索引2）
	switch (vofaCommandData.uartRxPacket[2])
	{
		case '1': vofaCommandData.cmdID = Direct_Assignment; break;
		case '2': vofaCommandData.cmdID = Increase; break;
		case '3': vofaCommandData.cmdID = Decrease; break;
		default:  vofaCommandData.cmdID = INVALID; goto parse_fail;
	}

	// 解析数值（索引4 到 '!' 之前，ASCII 转 float）
	char numStr[16] = {0};
	int numLen = pEnd - (vofaCommandData.uartRxPacket + 4);
	if (numLen <= 0 || numLen >= 16)
	{
		goto parse_fail;
	}
	memcpy(numStr, vofaCommandData.uartRxPacket + 4, numLen);
	numStr[numLen] = '\0';
	vofaCommandData.floatData = (float)strtof(numStr, NULL);

	memset(vofaCommandData.uartRxPacket, 0, CMD_FRAME_SIZE);
	return;

parse_fail:
	memset(vofaCommandData.uartRxPacket, 0, CMD_FRAME_SIZE);
	vofaCommandData.cmdType = INVALID;
	vofaCommandData.cmdID = INVALID;
}

/* ========== 回调机制 ========== */

void vofaRegisterCallback(VofaCmdCallback cb)
{
	s_cmdCallback = cb;
}

uint8_t vofaGetCommand(void)
{
	if (vofaCommandData.completionFlag)
	{
		vofaCommandParse();

		if (s_cmdCallback != NULL &&
		    vofaCommandData.cmdType != INVALID &&
		    vofaCommandData.cmdID != INVALID)
		{
			s_cmdCallback(vofaCommandData.cmdType, vofaCommandData.cmdID,
			              vofaCommandData.floatData);
		}
		vofaCommandData.completionFlag = 0;  // 清除标志
		return 1;
	}
	return 0;
}