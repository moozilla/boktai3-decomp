#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0822EA14(s32);
s32 sub_0822ECA8(void)
{
    if (Script_SeekToKeyword(0x77)) {
        sub_0822EA14(Script_GetValue());
        return 1;
    }
    return 0;
}
