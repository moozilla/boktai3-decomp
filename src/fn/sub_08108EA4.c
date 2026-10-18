#include "global.h"

void Serial_StopCommunication(void);

s32 LinkHandshake_OnDestroy(void *p)
{
    if (*(u32 *)((u8 *)p + 0x30) == 0) {
        Serial_StopCommunication();
    }
    return *(u32 *)0x020004B0 = 0;
}
