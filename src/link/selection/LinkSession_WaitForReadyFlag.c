#include "global.h"
void LinkSession_ConsumeStateEntry(u8 *);
void LinkSession_SetState(u8 *, u32, void (*)(void));
void SceneMenu_WaitForConfirmation(void);
u32 LinkSession_WaitForReadyFlag(u8 *p)
{
    u32 v;
    LinkSession_ConsumeStateEntry(p);
    v = p[0x2a];
    if (v != 0)
        LinkSession_SetState(p, 0x1a, SceneMenu_WaitForConfirmation);
}
