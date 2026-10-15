#include "script_utility.h"
#define ScriptUtility_GetKeys sub_08056BC8
u32 ScriptUtility_GetKeys(void)
{
    if (Script_SeekToKeyword('p')) {
        u32 keys = gUnk_03005260[1];
        return keys;
    }
    if (Script_SeekToKeyword('s'))
        return gUnk_03005260[0];
    {
        u32 keys;
        if (Script_SeekToKeyword('r') == 0)
            keys = gUnk_03005260[1];
        else
            keys = gUnk_03005260[2];
        return keys;
    }
}
