#include "global.h"
void Script_Call(u32, u32);
void FragmentBurst_RunCompletionScript(u8 *p)
{
    if (*(u32 *)(p + 0x158))
        Script_Call(*(u32 *)(p + 0x158), 0);
}
