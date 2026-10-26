#include "global.h"
#include "ram.h"
u32 ScriptProc_Start(u32, u32);
u32 FragmentProjectilePool_Init(u8 *p)
{
    *(u32 *)(p + 0x18) = ScriptProc_Start(0x922E, 0x1DC3);
    *(u32 *)(p + 0x1c) = 0;
    gActorEffectManager = p;
    return 0;
}
