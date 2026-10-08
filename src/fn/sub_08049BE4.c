#include "global.h"
extern u8 *gUnk_02000488;
void sub_08049168(u8 *, u32);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u32 sub_0821FCDC(u8 *);
void sub_0821FD3C(u8 *, u8 *);
u32 sub_0821F2EC(u8 *, u8 *, u8 *);
void sub_08049BE4(void)
{
    u8 *p = gUnk_02000488;
    if (p) {
        u8 *a;
        u8 *b;
        if (Script_SeekToKeyword(0x70)) {
            *(u16 *)(p + 0x410) = Script_GetValue();
            *(u16 *)(p + 0x412) = Script_GetValue();
            *(u16 *)(p + 0x414) = Script_GetValue();
        }
        if (Script_SeekToKeyword(0x52))
            *(u32 *)(p + 0x418) = Script_GetValue();
        if (p[0x255] == 3) {
            a = p + 0x50;
            if (sub_0821FCDC(a) == 0xFF || (b = p + 0x1D8, sub_0821FD3C(b, a), sub_0821F2EC(b, a, p + 0x410) == 0))
                sub_08049168(p, 0);
            else
                sub_08049168(p, 3);
        }
    }
}
