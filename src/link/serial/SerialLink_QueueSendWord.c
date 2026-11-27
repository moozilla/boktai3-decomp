#include "global.h"

void Serial_DisableGlobalInterrupts(void);
void Serial_EnableGlobalInterrupts(void);

s32 SerialLink_QueueSendWord(u16 v)
{
    s32 r;
    Serial_DisableGlobalInterrupts();
    if (*(u16 *)0x03001728 & 0x4000) {
        *(u16 *)0x03001728 = v;
        r = 0;
    } else {
        r = -4;
    }
    Serial_EnableGlobalInterrupts();
    return r;
}
