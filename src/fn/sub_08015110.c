#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_080150D0(void *, void *);
void sub_0821A0C0(void *);
void sub_08015040(void);
void sub_08015074(void);

void *sub_08015110(void *a)
{
    void *p = sub_08219FBC(10, 0x600);
    if (p != 0) {
        sub_0821A04C(p, sub_08015040, sub_08015074);
        if (sub_080150D0(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
