#include "global.h"
u32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
void sub_0822D404(void);
void sub_0822D42C(void);
void sub_0822CEE8(s32, s32);
void sub_0822D448(void)
{
    s32 n;
    sub_0822D404();
    sub_0822D42C();
    if (Script_SeekToKeyword(0x6e))
        n = Script_GetValue();
    else
        n = 0;
    if (n > 0 && Script_SeekToKeyword(0x69)) {
        s32 i;
        for (i = 0; i < n; i++)
            sub_0822CEE8(Script_GetValue(), 0);
    }
}
