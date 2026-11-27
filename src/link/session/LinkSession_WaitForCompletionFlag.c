#include "global.h"
void LinkSession_ConsumeStateEntry(u8 *);
void LinkSession_SetState(u8 *, u32, void (*)(void));
void LinkSession_EnterCompletedState(void);
u32 LinkSession_WaitForCompletionFlag(u8 *p)
{
    u32 v;
    LinkSession_ConsumeStateEntry(p);
    v = p[0x2B];
    if (v != 0)
        LinkSession_SetState(p, 0x1C, LinkSession_EnterCompletedState);
}
