#include "global.h"
struct P { u8 pad[0x8c]; s32 v; u8 pad2[0x1a]; u16 t; u8 pad3[3]; u8 st; };
void sub_0801D354(struct P *, s32);
u32 sub_0801D3CC(struct P *p)
{
    switch (p->st) {
    case 0:
        p->st++;
        p->t = 0;
        break;
    case 1:
    case 3:
    case 5:
        p->t++;
        if ((p->t & 3) == 0) {
            sub_0801D354(p, 1);
        } else {
            sub_0801D354(p, 0);
        }
        if (p->v > 0x1000) {
            p->st++;
        }
        break;
    case 2:
    case 4:
    case 6:
        sub_0801D354(p, 0);
        if (p->v <= 0xBFF) {
            p->st++;
            if (p->st > 5) return 1;
        }
        break;
    default:
        break;
    }
    return 0;
}
