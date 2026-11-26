#include "global.h"
void Registry1745_ConsumeStateEntry(u8 *);
void Registry1745_SetState(u8 *, u32, void (*)(void));
void SceneMenu_WaitForConfirmation(void);
u32 Registry1745_WaitForReadyFlag(u8 *p)
{
    u32 v;
    Registry1745_ConsumeStateEntry(p);
    v = p[0x2a];
    if (v != 0)
        Registry1745_SetState(p, 0x1a, SceneMenu_WaitForConfirmation);
}
