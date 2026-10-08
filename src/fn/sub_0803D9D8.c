#include "global.h"
struct P { u8 pad[0x14]; u32 w14; u8 pad1[0x1c]; u32 w34; u8 pad2[0xc]; u32 w44; u8 pad3[0x10]; s32 w58; u8 pad4[8]; u8 b64; u8 pad5[3]; u32 w68; u8 b6c[1]; };
void sub_08219DD8(u8 *, u32);
u32 sub_0803D9D8(struct P *p)
{
    if (p->w14 >= p->w34) {
        if (p->w58 < 0) {
            p->w68 = 8;
            p->w44 = 0;
            p->b64 = 1;
            sub_08219DD8(p->b6c, 0x100);
        } else {
            p->w68 = 9;
            p->w44 = 0;
            p->b64 = 1;
            sub_08219DD8(p->b6c, 0x100);
        }
        return 1;
    }
    return 0;
}
