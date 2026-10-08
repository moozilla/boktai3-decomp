#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
void sub_0821A0C0(void *);
s32 sub_08225A3C(void *, u32, u32);
void sub_08225A2C(void);
void sub_08225A30(void);
extern void *gUnk_030025F8;
void *sub_08225A50(u32 a, u32 b)
{
    void *p;
    if (gUnk_030025F8 != 0)
        return gUnk_030025F8;
    p = sub_08219FBC(2, 0x20);
    if (p) {
        sub_0821A04C(p, sub_08225A2C, sub_08225A30);
        if (sub_08225A3C(p, a, b) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
