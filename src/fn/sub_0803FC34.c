#include "modifier_list.h"

void sub_0811E5D0(u8 *, s32);

void Modifier_LoadSecondSource(struct ModifierSelection *p, const struct ModifierSelection *current)
{
    u8 buf[8];
    u8 *b = buf;
    sub_0811E5D0(buf, p->sourceIds[1]);
    p->values0[2] = b[1];
    p->values0[3] = b[2];
    p->values0[4] = *(u16 *)(b + 4);
    p->capacity = b[3];
}
