#include "global.h"
struct E { u8 f[0x54]; };
extern s32 gUnk_020005AC;
void sub_08021578(void);
void sub_082195E0(void *);
void sub_08013B74(void *);
void sub_0821FE6C(void *);
s32 sub_081A2C3C(u8 *p)
{
    struct E *e;
    s32 i;
    s32 z;
    sub_08021578();
    sub_082195E0(p + 0x194);
    sub_08013B74(p + 0x3b4);
    e = (struct E *)(p + 0x248);
    i = 1;
    do {
        sub_0821FE6C(e);
        e++;
        i--;
    } while (i >= 0);
    z = 0;
    gUnk_020005AC = z;
    return 0;
}
