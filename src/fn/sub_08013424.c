#include "global.h"

void Callback_InvokeActor(u8 *p);
void Shadow_UpdateTerrain(void);

void ActorShadow_Enable(u8 *p)
{
    *(u32 *)(p + 0xC) &= ~1;
    *(u32 *)(p + 0x34) = (u32)Shadow_UpdateTerrain;
    Callback_InvokeActor(p);
}
