#ifndef VOFA_FUNCTIONS_H__
#define VOFA_FUNCTIONS_H__

#define INVALID 0xFF
#define FRAME_TAIL_SIZE (4U)
#define INCREASE_STEP (0.01f)
#define DECREASE_STEP (0.01f)
#define CH_COUNT (8U)
#define CMD_FRAME_SIZE 10

#include <stdint.h>

enum CommandID
{
	Direct_Assignment,
	Increase,
	Decrease
};

enum CommandType
{
	Speed,
	Position
};

typedef struct vofaJustFloatFrame
{
	float   fdata[CH_COUNT];
	uint8_t frametail[FRAME_TAIL_SIZE];
} vofaJustFloatFrame;

typedef struct vofaCommand
{
	uint8_t cmdType;
	uint8_t cmdID;
	uint8_t validData[4];
	uint8_t uartRxPacket[CMD_FRAME_SIZE];
	uint8_t completionFlag;
	float   floatData;
} vofaCommand;

/* 命令回调函数类型：把解析好的命令交给上层处理 */
typedef void (*VofaCmdCallback)(uint8_t cmdType, uint8_t cmdID, float value);

#ifdef __cplusplus
extern "C" {
#endif

void vofaSendJustFloat(vofaJustFloatFrame* vofaJFFrame);
void vofaSendFirewater(const float* fdata, uint32_t ulSize);
void vofaSendRawdata(uint8_t* pData, uint32_t ulSize);

void vofaJustFloatInit(void);
void uartCMDRecv(uint8_t byte_data);
void vofaCommandParse(void);

/* 注册命令回调，上层调用这个设置处理函数 */
void vofaRegisterCallback(VofaCmdCallback cb);
/* 带回调的获取命令，解析后自动调用回调 */
uint8_t vofaGetCommand(void);

extern vofaJustFloatFrame JustFloat_Data;
extern vofaCommand        vofaCommandData;

#ifdef __cplusplus
}
#endif

#endif