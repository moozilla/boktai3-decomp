#include "global.h"

struct Unk0810C1E8_Elem { u32 flags; u8 filler4[0x5C]; };
struct Unk0810C1E8 {
    u8 filler0[0x28];
    struct Unk0810C1E8_Elem elems[7];
    u8 filler2C8[0x38];
    u16 vals[8]; // 0x300
};

void sub_0810C210(struct Unk0810C1E8 *p, s32 i)
{
    p->elems[i].flags &= ~1;
    p->vals[i] = 0x2a;
}
