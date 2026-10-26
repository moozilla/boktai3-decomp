#include "script_utility.h"
void Sound_RestartPlayer30Song(void);
void ScriptUtility_ClearControlFlag(void)
{
    gUnk_030053F4 &= ~0x200;
    if (gEnginePauseFlags & 8) {
        Sound_RestartPlayer30Song();
        {
            u32 mask = ~8;
            gEnginePauseFlags &= mask;
        }
    }
}
