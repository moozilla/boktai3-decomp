#include "global.h"
#include "ram.h"

u32 ScriptProc_Start(u32 a, u32 b);

u32 ScriptSpriteManager_Init(u8 *p)
{
    gColliderSpritePool = p;
    *(u32 *)(p + 0x18) = ScriptProc_Start(0x922E, 0x931E);
    *(u32 *)(p + 0x1C) = 0;
    return 0;
}
