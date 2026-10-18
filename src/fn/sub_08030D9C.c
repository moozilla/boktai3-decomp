#include "global.h"

s32 Script_SeekToKeyword(s32);
u8 *Script_GetPc(void);
u32 Script_GetTextSourcePc(void);
u32 MessageWindow_SetMessageTable(u32);
u32 MessageWindow_SetMessageTableFromScript(void)
{
    u32 v;
    if (Script_SeekToKeyword(0x72) == 0)
        v = (u32)Script_GetPc();
    else
        v = Script_GetTextSourcePc();
    return MessageWindow_SetMessageTable(v);
}
