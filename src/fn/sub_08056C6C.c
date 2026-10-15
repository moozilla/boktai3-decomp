#include "script_utility.h"
#define ScriptUtility_SetControlFlag sub_08056C6C
void sub_0822B548(void);
void ScriptUtility_SetControlFlag(s32 stopSong)
{
    gUnk_030053F4 |= 0x200;
    if (stopSong && !(gUnk_0300523C & 8)) {
        sub_0822B548();
        gUnk_0300523C |= 8;
    }
}
