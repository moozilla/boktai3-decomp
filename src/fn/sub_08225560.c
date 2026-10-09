#include "global.h"
u32 Script_GetValue(void);
typedef void (*Callback)(u16, u32);
Callback sub_08225884(u16);
s32 sub_08225560(void)
{
    Callback entry = sub_08225884(Script_GetValue());
    s32 result;
    if (entry) {
        entry(Script_GetValue(), 0);
        result = 0;
    } else
        result = -1;
    return result;
}
