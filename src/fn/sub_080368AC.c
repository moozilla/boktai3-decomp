#include "global.h"
struct P { u8 pad[0x1c]; u8 b1c; u8 b1d; u8 pad1[0x14]; u8 b32; u8 b33; u8 pad2[4]; u16 h38; u8 pad3[10]; u32 w44; u32 w48; u32 w4c; u8 pad5[0x338]; u32 w388; };
extern u32 gUnk_020000F0;
s32 sub_08224D3C(void);
void sub_080340AC(struct P *);
u32 sub_0821ABA8(u32, u32);
s32 Script_SeekToKeyword(s32);
u32 sub_08227E90(void);
s32 sub_080368AC(struct P *p)
{
    u32 z;
    u8 *f = &p->b33;
    z = 0;
    *f = z;
    gUnk_020000F0 = z;
    if (sub_08224D3C() < 0) goto fail;
    p->b1d = z;
    sub_080340AC(p);
    {
        u32 one = 1;
        p->b1c = z;
        p->h38 = one;
        p->w44 = z;
        p->b32 = 1;
    }
    p->w4c = sub_0821ABA8(0x70, 0);
    if (!Script_SeekToKeyword(0x61)) goto fail;
    {
        u32 v = sub_08227E90();
        p->w388 = v;
        if (v == 0) {
        fail:
            return -1;
        }
    }
    return 0;
}
