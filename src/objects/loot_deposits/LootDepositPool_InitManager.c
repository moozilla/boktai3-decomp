#include "global.h"
#include "engine/ram.h"

u32 ScriptProc_Start(u32, u32);

u32 LootDepositPool_InitManager(u8 *p)
{
    *(u32 *)(p + 0x18) = ScriptProc_Start(0x922E, 0x74C9);
    gActorGroupPool = p;
    return *(u32 *)(p + 0x62C) = 0;
}
