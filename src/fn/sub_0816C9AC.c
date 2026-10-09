#include "global.h"
extern u8 *gUnk_02000710;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0816C9AC(void) {
    s32 n;
    u32 *p;
    if (Script_SeekToKeyword(0x69) == 0) return;
    n = Script_GetValue();
    p = (u32 *)(gUnk_02000710 + 0x5bc);
    *p |= 1 << n;
}
