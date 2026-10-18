#include "global.h"
u32 Script_SeekToKeyword(u32);
u32 Script_GetValue(void);
void sub_082261B8(u32, u32);
void Camera_SetScreenOffsetFromScript(void)
{
    if (Script_SeekToKeyword(0x6f)) {
        u32 a = Script_GetValue();
        sub_082261B8(a, Script_GetValue());
    }
}
