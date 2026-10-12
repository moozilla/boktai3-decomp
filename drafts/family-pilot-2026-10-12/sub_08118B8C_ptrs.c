#include "global.h"

/* Field offsets relative to the embedded +0x18 object.  The movement helper
 * sub_0811AE9C consumes +0x10..+0x28 and resets +0x40; no public semantic
 * type or function names are claimed by this local layout. */
typedef struct Core {
    u32 unk0, unk4, unk8;
    u8 padC[4];
    s32 unk10, unk14, unk18;
    u8 pad1C[4];
    s32 unk20, unk24, unk28;
    u8 pad2C[12];
    s32 unk38, unk3C;
    u32 unk40;
    u8 unk44[0x18];
    u32 unk5C;
    u8 pad60[0x60];
    s32 unkC0;
    u8 padC4[0x4E8];
    s32 unk5AC, unk5B0, unk5B4;
    u8 pad5B8[4];
    s32 unk5BC;
    u8 pad5C0[4];
    s32 unk5C4;
    u8 pad5C8[4];
    s32 unk5CC, unk5D0, unk5D4;
    u8 pad5D8[4];
    s32 unk5DC;
    u8 pad5E0[0xC];
    s8 unk5EC;
    u8 pad5ED[3];
    u16 unk5F0;
    u8 pad5F2[6];
} Core;
typedef struct Obj {
    u8 pad0[0x18];
    Core core;
    u8 unk610[0x254];
    u32 unk864;
    u8 unk868;
    u8 pad869[7];
    u8 unk870[0x78];
    s32 unk8E8;
    u8 pad8EC[8];
    u16 unk8F4;
} Obj;
void sub_081188C8(Obj *, u32);
void sub_08020D68(void *, u32);
void sub_081167CC(Obj *);
void sub_0811CFAC(Core *);
s32 Div(s32, s32);
void sub_08219DD8(void *, u32);
void sub_0811BCBC(Core *);
s32 sub_082215E4(s32, s32);
void sub_0811AE9C(Core *);
void sub_0811D91C(Core *);
void sub_0811BD30(Core *, void *);
void sub_0811BF44(Core *);
void sub_0811CCB0(void *);
void sub_0811C2EC(Core *, void *, u32);

void sub_08118B8C(Obj *s)
{
    Core *p = &s->core;
    s32 x, a, b, c;
    u32 d, e;
    s32 *px, *ox, *vy, *vz, *oz;
    if (s->unk868) {
        sub_081188C8(s, 0x20);
        sub_08020D68(s->unk870, 1);
        s->unk868 = 0;
    }
    s->unk8F4 |= 0x10;
    sub_081167CC(s);
    sub_0811CFAC(p);
    x = s->core.unk5AC + s->core.unkC0;
    if (x > 0xF000) x = 0xF000;
    s->core.unk5B0 += 0x200;
    if (s->core.unk5EC > 0) {
        s->core.unk5DC += s->core.unk5CC;
        s->core.unk5CC += Div(-s->core.unk5CC, s->core.unk5EC);
        s->core.unk5D0 += Div(-s->core.unk5D0, s->core.unk5EC);
        s->core.unk5D4 += Div(-s->core.unk5D4, s->core.unk5EC);
        s->core.unk5EC--;
    } else {
        sub_08219DD8(&s->core.unk5CC, 0x10);
    }
    sub_0811BCBC(p);
    px = &p->unk5DC;
    ox = &p->unk5BC;
    p->unk20 = x + *px - *ox;
    vy = &p->unk5B0;
    p->unk24 = *vy + p->unk5D0;
    vz = &p->unk5B4;
    p->unk28 = *vz + p->unk5D4;
    a = (*px - *ox) >> 12;
    oz = &p->unk5C4;
    b = (p->unk18 - *oz) >> 12;
    if (x > 0x3000) a = 0x80;
    if (a == 0 && b == 0) p->unk5F0 = 0xFFFF;
    else p->unk5F0 = (sub_082215E4(a, b) >> 5) << 5;
    *ox = *px;
    *oz = p->unk18;
    sub_0811AE9C(p);
    d = (p->unk3C >> 12) >= 0;
    e = (p->unk38 >> 12) >= 0;
    if (p->unk4 != p->unk8 && (p->unk4 & 0x1000) && !(p->unk8 & 0xF000) && e <= 1 && d == 1) {
        c = ((p->unk5AC >> 10) + (p->unkC0 >> 8)) >> 1;
        if (c < 4) c = 4;
        if (c > 18) c = 18;
        *vy -= c << 9;
        p->unk40 &= ~1;
    }
    s->unk8E8 = x;
    sub_0811D91C(p);
    sub_0811BD30(p, s->unk610);
    if (p->unk40 & 4) sub_0811BF44(p);
    if (p->unk40 & 2) *vz = 0;
    if (p->unk40 & 1) *vy = 0;
    sub_0811CCB0(s->unk610);
    sub_0811C2EC(p, p->unk44, p->unk5C);
    s->unk864++;
}
