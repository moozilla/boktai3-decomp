#include "global.h"
extern s16 gUnk_0203B400[1024];
extern u32 gUnk_03005308, gUnk_03005304;
s32 sub_08219C00(void);
void sub_082210B8(void)
{
    s32 i;
    for (i = 0; i <= 1023; i++) {
        s32 value = sub_08219C00();
        do { gUnk_0203B400[i] = value >> (sub_08219C00() & 3); } while (0);
    }
    gUnk_03005308 = sub_08219C00() & 1023;
    gUnk_03005304 = sub_08219C00() & 1023;
}
