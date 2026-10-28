#include "script_selector.h"
#include "render.h"
void ScriptSelector_MoveSelection(struct ScriptSelector *, s32);
s32 ScriptSelector_InitializeTask(struct ScriptSelector *selector)
{
    s32 result;
    gBgFadeLevel = 0x40;
    if (!Script_SeekToKeyword('s'))
        goto failed;
    {
        u8 *resource;
        selector->scriptStart = (u32)Script_GetPc();
        selector->index = 0;
        selector->lastDirection = 0;
        ScriptSelector_MoveSelection(selector, 0);
        resource = ScriptProc_Start(0x92B3, 0x519C);
        CpuSet(resource + 0x1F4, gBgPaletteBank15, 0x04000008);
        result = 0;
        goto done;
    }
failed:
    result = -1;
done:
    return result;
}
