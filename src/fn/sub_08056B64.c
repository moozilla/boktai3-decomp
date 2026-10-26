#include "script_utility.h"
s32 ScriptUtility_GetRandom(void)
{
    if (Script_SeekToKeyword('p')) {
        s32 bound = Script_GetValue();
        if (bound > 0) {
            gRandomIndex = (gRandomIndex + 1) & 1023;
            {
                u32 offset = gRandomIndex * 2;
                s32 sample = *(u16 *)(offset + RANDOM_TABLE_ADDR);
                return Mod(sample >> 3, bound);
            }
        }
    }
    gRandomIndex = (gRandomIndex + 1) & 1023;
    {
        u32 offset = gRandomIndex * 2;
        return *(u16 *)(offset + RANDOM_TABLE_ADDR);
    }
}
