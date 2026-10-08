#include "global.h"
extern void *gUnk_02000240;
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_081A46B0(void);
void sub_081A46EC(void);
s32 sub_081A4760(void *);
void *sub_081A47E4(void)
{
    void *p;
    if (gUnk_02000240 == 0) {
        gUnk_02000240 = sub_08219FBC(8, 0xBDC);
        p = gUnk_02000240;
        if (p != 0) {
            sub_0821A04C(p, sub_081A46B0, sub_081A46EC);
            if (sub_081A4760(p) < 0) {
                sub_0821A0C0(p);
                return 0;
            }
        }
        return p;
    }
    return gUnk_02000240;
}
