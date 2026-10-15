#include "script_utility.h"
#define ScriptUtility_GetRandom sub_08056B64
s32 ScriptUtility_GetRandom(void)
{
    if (Script_SeekToKeyword('p')) {
        s32 bound = Script_GetValue();
        if (bound > 0) {
            gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
            {
                u32 offset = gUnk_03005308 * 2;
                s32 sample = *(u16 *)(offset + RANDOM_TABLE_ADDR);
                return Mod(sample >> 3, bound);
            }
        }
    }
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    {
        u32 offset = gUnk_03005308 * 2;
        return *(u16 *)(offset + RANDOM_TABLE_ADDR);
    }
}
