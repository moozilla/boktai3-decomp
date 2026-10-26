#include "script_utility.h"
void Sound_SuspendPlayer30Song(void);
void ScriptUtility_SetControlFlag(s32 stopSong)
{
    gEngineFlags |= 0x200;
    if (stopSong && !(gEnginePauseFlags & 8)) {
        Sound_SuspendPlayer30Song();
        gEnginePauseFlags |= 8;
    }
}
