#include "global.h"
extern u32 gUnk_03002604;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_0822CC60(void *);
void sub_0821A0C0(void *);
void sub_0822CA28(void);
void sub_0822CA48(void);

void *sub_0822CC80(void)
{
    void *p;
    if (gUnk_03002604 != 0)
        return (void *)gUnk_03002604;
    p = sub_08219FBC(5, 0x30);
    if (p != 0) {
        sub_0821A04C(p, sub_0822CA28, sub_0822CA48);
        if (sub_0822CC60(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
