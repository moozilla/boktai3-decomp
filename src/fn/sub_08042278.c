#include "script_selector.h"
#define ScriptSelector_InitializeTask sub_08042278
extern u32 gUnk_03004DA0;
extern u32 gUnk_030051A0[];
void sub_08042198(struct ScriptSelector *, s32);
s32 ScriptSelector_InitializeTask(struct ScriptSelector *selector)
{
    s32 result;
    gUnk_03004DA0 = 0x40;
    if (!Script_SeekToKeyword('s'))
        goto failed;
    {
        u8 *resource;
        selector->scriptStart = (u32)Script_GetPc();
        selector->index = 0;
        selector->lastDirection = 0;
        sub_08042198(selector, 0);
        resource = ScriptProc_Start(0x92B3, 0x519C);
        CpuSet(resource + 0x1F4, gUnk_030051A0, 0x04000008);
        result = 0;
        goto done;
    }
failed:
    result = -1;
done:
    return result;
}
