#include "global.h"

struct State0819594C {
    u8 pad0[0x7255];
    u8 dead[9];
    u8 pad725e[0x726a - 0x725e];
    u16 maxHp;
    u8 pad726c[0x7278 - 0x726c];
    u16 hp[9];
};

void sub_0819594C(struct State0819594C *t)
{
    s32 i;
    for (i = 0; i <= 3; i++) {
        t->hp[i] = t->maxHp;
        t->dead[i] = 0;
    }
    for (; i <= 7; i++) {
        t->hp[i] = 0;
        t->dead[i] = 0;
    }
    t->hp[i] = t->maxHp;
    t->dead[i] = 0;
}
