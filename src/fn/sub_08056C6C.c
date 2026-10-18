#include "script_utility.h"
void Sound_SuspendPlayer30Song(void);
void ScriptUtility_SetControlFlag(s32 stopSong)
{
    gUnk_030053F4 |= 0x200;
    if (stopSong && !(gUnk_0300523C & 8)) {
        Sound_SuspendPlayer30Song();
        gUnk_0300523C |= 8;
    }
}
