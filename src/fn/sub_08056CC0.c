#include "script_utility.h"
void Sound_RestartPlayer30Song(void);
void ScriptUtility_ClearControlFlag(void)
{
    gUnk_030053F4 &= ~0x200;
    if (gUnk_0300523C & 8) {
        Sound_RestartPlayer30Song();
        {
            u32 mask = ~8;
            gUnk_0300523C &= mask;
        }
    }
}
