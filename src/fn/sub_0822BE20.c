#include "global.h"
extern u8 *gUnk_030053F8;
u8 sub_0822BBD8(s32);
u8 sub_0822BCE4(s32);
u8 sub_0822BB7C(void);
/* Switch to the other of two slots (0/1) if both checks accept it. */
u8 sub_0822BE20(void)
{
    s32 other = 1 - gUnk_030053F8[8];
    if (sub_0822BBD8(other) && sub_0822BCE4(other)) {
        gUnk_030053F8[8] = other;
        return sub_0822BB7C();
    }
    return 0;
}
