#include "global.h"

struct B { u8 f[0x2c]; u32 x; };
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_0800266C(void *, u16, void *);
void sub_0821A0C0(void *);
void sub_08002408(void);
void sub_08002658(void);

void *sub_08002840(u16 a, struct B *b)
{
    void *p;
    if (b->x == 0)
        p = sub_08219FBC(9, 0xf8);
    else
        p = sub_08219FBC(0xb, 0xf8);
    if (p != 0) {
        sub_0821A04C(p, sub_08002408, sub_08002658);
        if (sub_0800266C(p, a, b) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
