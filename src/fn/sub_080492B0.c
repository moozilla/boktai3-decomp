#include "global.h"
extern u8 *gUnk_02000488;
void sub_08049168(u8 *, u32);
s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u32 sub_0821FCDC(u8 *);
void sub_0821FD3C(u8 *, u8 *);
u32 sub_0821F2EC(u8 *, u8 *, u8 *);
u32 sub_080492B0(u8 *p)
{
    u8 *a = p + 0x50;
    u8 *b;
    u32 r;
    if (sub_0821FCDC(a) == 0xFF || (b = p + 0x1D8, sub_0821FD3C(b, a), sub_0821F2EC(b, a, p + 0x410) == 0)) r = 0;
    else r = 1;
    return r;
}
