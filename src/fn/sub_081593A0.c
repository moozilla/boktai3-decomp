#include "global.h"

struct V081593A0 {
    u32 a;
    u32 b;
};
struct P081593A0 {
    u8 f0[0x30];
    struct V081593A0 f30;
    u8 f38[0xAC - 0x38];
    struct V081593A0 fAC;
    u8 fB4[0x168 - 0xB4];
    struct V081593A0 f168;
};
struct H081593A0 {
    u16 a;
    u16 b;
    u16 c;
};
extern u8 *gGameState;

void Player_SetPositionAndSave(struct P081593A0 *p, struct H081593A0 *h)
{
    struct V081593A0 *v = (struct V081593A0 *)h;
    struct V081593A0 t = *v;
    p->f30 = t;
    p->f168 = t;
    p->fAC = t;
    *(u16 *)(gGameState + 0x30) = h->a;
    *(u16 *)(gGameState + 0x32) = h->b;
    *(u16 *)(gGameState + 0x34) = h->c;
}
