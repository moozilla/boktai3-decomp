#include "global.h"

void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08048248(void *, void *);
void sub_0821A0C0(void *);
void sub_080480A4(void);
void sub_0804822C(void);

void *sub_08048344(void *a)
{
    void *p = sub_08219FBC(8, 0xC4);
    if (p != 0) {
        sub_0821A04C(p, sub_080480A4, sub_0804822C);
        if (sub_08048248(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
