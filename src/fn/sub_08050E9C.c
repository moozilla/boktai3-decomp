#include "global.h"
extern u16 gUnk_02000528;
extern u16 gUnk_03004BD8;
void *ScriptProc_Start(u16, u16);
void sub_082161B4(u32, u32, void *, u32, u32, u32, void *);
void sub_08215A74(u32, u32);
void sub_08050E9C(u8 *p)
{
    u32 z[1];
    u32 f, v;
    *(void **)(p + 0x18) = ScriptProc_Start(0xC091, 0xB12);
    z[0] = 0;
    sub_082161B4(3, 2, *(void **)(p + 0x18), 0, 0, 1, z);
    sub_08215A74(1, 1);
    v = gUnk_02000528;
    f = 0;
    if (v == 1)
        f = 1;
    if (f != 0) {
        u16 *r = &gUnk_03004BD8;
        u32 m = ~0x200;
        *r = *r & m;
        p[0x37] = 1;
    } else {
        if (p[0x1f] != 0)
        {   u32 m = 0x200;
            gUnk_03004BD8 |= m; }
        else
        { u16 *r = &gUnk_03004BD8;
          u32 m = ~0x200;
          *r = *r & m; }
        p[0x37] = 0;
    }
}
