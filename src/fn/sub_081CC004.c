#include "global.h"
#define B(o) (*(u8 *)((u8 *)p + (o)))
#define H(o) (*(u16 *)((u8 *)p + (o)))
#define W(o) (*(u32 *)((u8 *)p + (o)))
struct S { u8 f[1]; u8 b1; u8 f2[5]; u8 b7; u8 b8; u8 b9; u8 g[0xd4 - 0xa]; u8 d4[1]; };
void sub_0802DCD8(void *, u32, u32, u32, u32);
s32 sub_080424FC(void *);
void sub_081CC004(u32 a, struct S *p)
{
    u32 r;
    if (p->b9 != 0) {
        p->b9 = 0;
        p->b7 = 0;
        switch (p->b1) {
        case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: r = 0; break;
        case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: r = 0; break;
        case 29: r = 0xa; break;
        default: r = 0; break;
        }
        sub_0802DCD8(p, r, 2, 1, 4);
        { u32 z = 0; B(0x18) = z; W(0x10) = z; }
    }
    if (sub_080424FC(p->d4) != 0) p->b7 = 1;
}
