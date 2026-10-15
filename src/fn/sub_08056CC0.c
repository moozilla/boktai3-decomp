#include "script_utility.h"
void sub_0822B560(void);
void ScriptUtility_ClearControlFlag(void)
{
    gUnk_030053F4 &= ~0x200;
    if (gUnk_0300523C & 8) {
        sub_0822B560();
        {
            u32 mask = ~8;
            gUnk_0300523C &= mask;
        }
    }
}
