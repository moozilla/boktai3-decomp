#include "global.h"

u32 ScriptProc_Start(u32, u32);

u32 CannonBattery_LoadAnimation(void)
{
    return ScriptProc_Start(0x922E, 0xEBB6);
}
