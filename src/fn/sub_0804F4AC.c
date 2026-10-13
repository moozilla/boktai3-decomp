#include "global.h"
struct Z {
    u32 x, y;
};
struct C {
    u32 w[8];
};
struct C *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(u8 *, struct C *);
void sub_0821983C(u8 *, u8 *, u32, u32, u32, u32, u32, struct Z *);
void sub_0804F4AC(u8 *p)
{
    struct Z z;
    struct C *r = ScriptProc_Start(0xCB05, 0x530D);
    if (r != 0) {
        u8 *a, *b;
        s32 j;
        *(struct C *)(p + 0x60) = *r;
        a = p + 0x60;
        ResourceHeader_Relocate(a, r);
        {
            u32 m = 0xFFFF0000;
            z.x = 0;
            z.y &= m;
        }
        b = a;
        a += 0x20;
        j = 3;
        do {
            sub_0821983C(a, b, 0x1e, 0x31, 1, 0, 0x3c, &z);
            a += 0x60;
            j--;
        } while (j >= 0);
    }
}
