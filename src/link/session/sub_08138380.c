#include "global.h"
void LinkSession_SetState(void *, u32, u32);
void StageIconDisplay_RequestKill(void);
void Serial_StopCommunication(void);
void LinkSession_BeginShutdown(u8 *p, u32 b)
{
    u8 t;
    *(u16 *)(p + 0x1a) = 0;
    t = p[0x1c];
    p[0x20] = t;
    LinkSession_SetState(p, 0, b);
    StageIconDisplay_RequestKill();
    Serial_StopCommunication();
}
