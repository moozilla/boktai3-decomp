#include "global.h"
#include "engine.h"
void *sub_081D4F20(s32 k, s32 c);
void sub_081D5188(void *, s32 i);
/* Script: sub_081D5188(sub_081D4F20(-k, -c), -i), all default 0 (tentative). */
void RegistryA76D_SetFromScript(void)
{
    s32 k, c, i;
    if (Script_SeekToKeyword('k'))
        k = Script_GetValue();
    else
        k = 0;
    if (Script_SeekToKeyword('c'))
        c = Script_GetValue();
    else
        c = 0;
    if (Script_SeekToKeyword('i'))
        i = Script_GetValue();
    else
        i = 0;
    sub_081D5188(sub_081D4F20(k, c), i);
}
