#include "global.h"
extern const u32 gUnk_08611E1C[];
struct T { u8 p0[0x90]; u32 w90; u8 p1[0xa2-0x94]; u8 ba2; u8 p2; u32 wa4; u8 p3[0xaa-0xa8]; u8 baa; u8 bab; u8 p4[0xb1-0xac]; u8 bb1; u8 p5[0x100-0xb2]; u32 w100; };
u32 sub_081A1EA4(struct T *p)
{
    if ((u8)(p->baa - 1) > 0x26) return 0;
    {
        u32 v = gUnk_08611E1C[p->baa];
        p->bab = p->baa;
        p->w100 = v;
        p->w90 = 0;
        p->bb1 = 1;
        p->ba2 = 0;
        p->wa4 = 0;
        return 1;
    }
}
