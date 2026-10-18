#include "global.h"
#include "scene_hud_display.h"
#define Script_GetArgumentPc Script_GetTextSourcePc
u32 Script_GetTextSourcePc(void);
s32 Script_GetValueSafe(void);
void SceneHud_ReadScriptOptions(struct SceneHudState *p)
{
    if (Script_SeekToKeyword('i'))
        p->actionTextScript = (void *)Script_GetTextSourcePc();
    if (Script_SeekToKeyword('w'))
        p->timedMessageScript = (void *)Script_GetTextSourcePc();
    if (Script_SeekToKeyword('a'))
        p->optionAScript = (void *)Script_GetTextSourcePc();
    if (Script_SeekToKeyword('g'))
        p->unlockTextScript = (void *)Script_GetTextSourcePc();
    if (Script_SeekToKeyword('L'))
        p->actionDescriptionScript = (void *)Script_GetTextSourcePc();
    if (Script_SeekToKeyword('A'))
        p->optionUpperAScript = (void *)Script_GetTextSourcePc();
    if (Script_SeekToKeyword('l'))
        p->optionLScript = (void *)Script_GetTextSourcePc();
    if (Script_SeekToKeyword('E'))
        p->optionUpperEScript = (void *)Script_GetTextSourcePc();
    if (Script_SeekToKeyword('e'))
        p->optionEValue = Script_GetValueSafe();
}
