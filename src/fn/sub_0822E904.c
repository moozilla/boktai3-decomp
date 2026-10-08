#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0822E668(s32);
s32 sub_0822E904(void)
{
    if (Script_SeekToKeyword(0x77)) {
        sub_0822E668(Script_GetValue());
        return 1;
    }
    return 0;
}
