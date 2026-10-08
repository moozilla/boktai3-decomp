#include "global.h"

struct Unk081FC61C {
    u16 unk0;
    u16 unk2;
    u8 filler4[2];
    u8 unk6;
    u8 filler7;
    u32 unk8;
    u32 unkC;
    u8 filler10[8];
    u32 unk18;
};

void sub_08219DD8(void *, u32);
void sub_081FC618(void *, void (*)(void));
void sub_081FBE2C(void);

void sub_081FC61C(struct Unk081FC61C *p)
{
    sub_08219DD8(p, 0x1C);
    sub_081FC618(p, sub_081FBE2C);
    p->unk0 = 0;
    p->unk2 = 20;
    p->unk8 = 5;
    p->unk6 = 0;
    p->unk18 = 0;
    p->unkC = 0x200;
}
