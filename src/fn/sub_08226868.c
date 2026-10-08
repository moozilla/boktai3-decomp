#include "global.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
extern void *gUnk_03005420;
void sub_082265F8(void *);
void sub_08226474(s32);
void sub_08226868(void)
{
    if (Script_SeekToKeyword(0x66)) {
        s32 v = Script_GetValue();
        sub_082265F8(gUnk_03005420);
        sub_08226474(v);
    }
}
