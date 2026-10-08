#include "global.h"

extern void *gUnk_02000258;
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_081B8098(void *);
void sub_0821A0C0(void *);
void sub_081B7C98(void);
void sub_081B7D44(void);

void *sub_081B80BC(void)
{
    void *r = sub_08219FBC(8, 0x4c0);
    if (r != 0) {
        sub_0821A04C(r, sub_081B7C98, sub_081B7D44);
        if (sub_081B8098(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    gUnk_02000258 = r;
    return r;
}
