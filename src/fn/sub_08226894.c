#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
extern void *gUnk_03005420;
void Camera_ReadMoveRate(void *);
void sub_082264D0(s32);
void sub_08226894(void)
{
    if (Script_SeekToKeyword(0x66)) {
        s32 v = Script_GetValue();
        Camera_ReadMoveRate(gUnk_03005420);
        sub_082264D0(v);
    }
}
