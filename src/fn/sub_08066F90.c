#include "global.h"

struct Ent {
    u8 pad0[0x3e];
    s16 a;
    u16 b;
    s16 c;
    u8 pad44[0x12];
    u16 d;
    u8 pad58[0x54];
};
struct Mgr {
    u8 pad[0x3c];
    struct Ent e[1];
};
extern struct Mgr *gUnk_02000128;

void sub_08066F90(s32 i)
{
    struct Mgr *m = gUnk_02000128;
    if (i >= 0) {
        struct Ent *e = &m->e[i];
        if (e->c >= 0 && e->a != 2) {
            e->b |= 2;
            e->d |= 4;
        }
    }
}
