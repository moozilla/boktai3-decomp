#include "global.h"

extern void *gUnk_03005420;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_08227478(void *, void *);
void sub_0821A0C0(void *);
void sub_08227378(void);
void sub_0822744C(void);

void *sub_082275D8(void *a)
{
    void *p;
    if (gUnk_03005420 != 0)
        return gUnk_03005420;
    p = sub_08219FBC(7, 0xA4);
    if (p != 0) {
        sub_0821A04C(p, sub_08227378, sub_0822744C);
        if (sub_08227478(p, a) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
