#include "global.h"

struct B {
    u8 f[0x38];
    u32 x;
};
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 Background_Initialize(void *, u16, void *);
void sub_0821A0C0(void *);
void Background_Update(void);
void sub_08001AD0(void);

void *sub_08001CB0(u16 a, struct B *b)
{
    void *p;
    if (b->x == 0)
        p = sub_08219FBC(9, 0x140);
    else
        p = sub_08219FBC(0xb, 0x140);
    if (p != 0) {
        sub_0821A04C(p, Background_Update, sub_08001AD0);
        if (Background_Initialize(p, a, b) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
