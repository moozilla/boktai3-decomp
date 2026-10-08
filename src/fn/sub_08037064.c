#include "global.h"
struct P { u8 pad[0x19]; u8 b19; u8 pad1[2]; u32 w1c; u8 pad2[0xc]; u32 w2c; };
void sub_082279A8(u32, u32, u32, u32, u32, u32, u32);
void sub_0822B2F8(u32);
void sub_08036C38(void);
void sub_0821447C(u32, void *, void *, void *);
void sub_0821AD08(u32, u32);
void sub_08217F4C(void);
void sub_08214B44(void);
void sub_08219624(void);
void sub_08037064(struct P *p)
{
    if (p->b19 != 0) {
        u32 z = 0;
        p->b19 = z;
        sub_082279A8(3, 4, 4, 4, 4, 0xFFFF, z);
        sub_0822B2F8(0xde);
        sub_08036C38();
    }
    if (p->w1c == 0x14) {
        sub_0821447C(0, sub_08217F4C, sub_08214B44, sub_08219624);
        if (p->w2c != 0) sub_0821AD08(p->w2c, 0);
    }
    p->w1c++;
}
