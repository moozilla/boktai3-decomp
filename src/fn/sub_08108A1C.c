#include "global.h"
#include "ram.h"

u32 ScriptProc_Start(u32, u32);

u32 AnimatedTile_InitManager(u8 *p)
{
    *(u32 *)(p + 0x18) = ScriptProc_Start(0x922E, 0x9AF2);
    gNamedEntryPool = p;
    return *(u32 *)(p + 0x154) = 0;
}
