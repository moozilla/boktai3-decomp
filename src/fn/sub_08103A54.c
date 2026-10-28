#include "global.h"
#include "ram.h"

u32 ScriptProc_Start(u32, u32);

s32 NamedObject_InitManager(void *p)
{
    *(u32 *)((u8 *)p + 0x18) = ScriptProc_Start(0x922E, 0x13F9);
    gNamedActorManager = p;
    return *(u32 *)((u8 *)p + 0x136C) = 0;
}
