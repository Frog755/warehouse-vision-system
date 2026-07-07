#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <unistd.h>

#define VOFA_UART_DEVICE    "/dev/ttyS0"
#define VOFA_UART_BAUDRATE  B115200

int  USART_Init(void);
void USART_DeInit(void);
void uartSendByte(uint8_t c);
void uartSendData(uint8_t* Array, uint8_t SIZE);
uint8_t USART_GetCommand(float* out_value);

extern uint8_t vofaRxBufferIndex;

#ifdef __cplusplus
}
#endif

#endif