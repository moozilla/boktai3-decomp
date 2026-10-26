#include "global.h"
#include "ram.h"

void AnimData_Load(u8 *, u32);
u32 ScriptProc_Start(u32, u32);

s32 Pool1B8_Init(u8 *p)
{
    AnimData_Load(p + 0x1C, 0xA945);
    *(u32 *)(p + 0x18) = ScriptProc_Start(0x922E, 0xAE9);
    gTileTriggerManager = p;
    return *(u32 *)(p + 0x68) = 0;
}
