#include "global.h"

u32 sub_0821F840(u16 *p);
void sub_08220D44(void *, u32);
void sub_08220F00(void *, u32);
s32 sub_0824923C(void *arg, void *function);
u8 sub_08078430(void *, void *, u32, u32, s32);
u8 sub_0807864C(void *, void *, u32, u32, s32);

struct Q {
    u8 pad[0x6d4];
    u16 unk6D4;
};

struct S {
    u8 pad0[4];
    u8 *unk4;
    u8 unk8;
    u8 pad9[0x260 - 9];
    void *unk260;
    u8 pad264[0x270 - 0x264];
    void *unk270;
    u8 pad274[0x2a8 - 0x274];
    u8 unk2A8;
    u8 unk2A9;
    u8 unk2AA;
    u8 pad2AB;
    s32 unk2AC;
    u8 pad2B0;
    u8 unk2B1;
    u8 pad2B2[0x2c2 - 0x2b2];
    u16 unk2C2;
    u8 pad2C4[0x2d4 - 0x2c4];
    u16 unk2D4;
    u8 pad2D6[0x3d0 - 0x2d6];
    struct Q *unk3D0;
    u8 pad3D4[0x5d0 - 0x3d4];
    u16 *unk5D0;
};

void sub_080AFD88(struct S *p)
{
    u8 *a = (u8 *)p + 0x3d4;
    struct Q *b = p->unk3D0;
    u16 *c;
    u32 r6;
    s32 r7;

    if (p->unk2B1 != 0) p->unk2B1 = 0;
    sub_0824923C(p, p->unk270);
    r7 = sub_0824923C(p, p->unk260);
    p->unk2C2 = 0x20;
    if (p->unk2D4 & 0xA02F) {
        p->unk2C2 = 0x40;
    } else if (p->unk2C2 == 0) {
        p->unk2C2 = 0x40;
    }
    c = &p->unk2C2;
    if (p->unk8 == 0) {
        sub_08220D44(p->unk4 + 0x48, *c);
    } else {
        sub_08220F00(p->unk4 + 0x20, *c);
    }
    a += 0x1fc;
    r6 = sub_0821F840(*(u16 **)a);
    if (r6 == 0) {
        if (sub_08078430(p, *(void **)a, 0x20, 0x20, r7)) {
            p->unk2A8 = 3;
            p->unk2A9 = r6;
            p->unk2AA = r6;
            p->unk2AC = r6;
            p->unk2B1 = 1;
            b->unk6D4 |= 2;
            return;
        }
    } else {
        if (sub_0807864C(p, *(void **)a, 0x20, 0x20, r7)) {
            p->unk2A8 = 0;
            p->unk2A9 = 0;
            p->unk2AA = 0x10;
            p->unk2AC = 0;
            p->unk2B1 = 1;
        }
    }
    p->unk2AC++;
}
