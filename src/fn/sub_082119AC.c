#include "global.h"
struct G { u8 pad[0x720]; u32 a720; u32 a724; u32 a728; u32 a72c; u32 a730; };
extern struct G *gUnk_02000710;
s32 Script_SeekToKeyword(s32);
u32 Script_GetValue(void);
void sub_082119AC(void)
{
    u32 n;
    u32 *p;
    if (Script_SeekToKeyword(0x69)) {
        n = Script_GetValue();
        if (n <= 0x1f) {
            u32 m;
            p = (u32 *)gUnk_02000710;
            p += 0x720 / 4;
            m = 1 << n;
            *p |= m;
        } else {
            u32 m;
            p = (u32 *)gUnk_02000710;
            p += 0x724 / 4;
            m = 1 << (n - 0x20);
            *p |= m;
        }
    }
}
