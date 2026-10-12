#include "global.h"

typedef struct Owner {
    u32 unk0;
    u8 pad4[0x1c];
    struct { u8 pad0[8]; u32 unk8; } inner;
} Owner;
typedef struct Obj {
    u8 pad0[0x4];
    Owner * unk4;
    u8 unk8;
    u8 pad9[0x50];
    u8 unk59;
    u8 pad5A[0x42];
    u16 unk9C;
    u8 pad9E[0x12];
    s16 unkB0;
    u8 padB2[0x2A];
    u32 unkDC;
    u8 padE0[0x38];
    u32 unk118;
    u32 unk11C;
    u8 unk120;
    u8 unk121;
    u8 unk122;
    u8 unk123;
    u32 unk124;
    u8 unk128;
    u8 pad129[0x3];
    u16 unk12C;
    u16 unk12E;
    u8 pad130[0x48];
    u16 unk178;
    u8 pad17A[0xA2];
    u32 unk21C;
    u8 pad220[0x88];
    u8 unk2A8;
    u8 unk2A9;
    u8 unk2AA;
    u8 pad2AB[0x5];
    u8 unk2B0;
    u8 unk2B1;
    u8 unk2B2;
    u8 unk2B3;
    u8 pad2B4[0x1];
    u8 unk2B5;
    u8 unk2B6;
    u8 pad2B7[0x9];
    u16 unk2C0;
    u16 unk2C2;
    u8 pad2C4[0x8];
    u32 unk2CC;
    u32 unk2D0;
    u16 unk2D4;
    u16 unk2D6;
    u32 unk2D8;
    u32 unk2DC;
    u32 unk2E0;
    u8 pad2E4[0x2];
    u16 unk2E6;
    u8 pad2E8[0x2];
    u16 unk2EA;
    u8 pad2EC[0x18];
    u32 unk304;
    u8 pad308[0x18];
    u8 unk320[0xac];
    u8 unk3CC;
} Obj;
void sub_08013424(void *);
void sub_080B1E60(Obj *);
void sub_08072188(void *);
static inline u8 take_flag(Obj *s)
{
    if (s->unk2B2) { s->unk2B2 = 0; return 1; }
    return 0;
}

void sub_080AD270(Obj *s)
{
    s32 n;
    if (take_flag(s)) {
        sub_08013424(s->unk320);
        if (!s->unk8) {
            u32 *q = &s->unk4->unk0;
            u32 mask = ~1;
            *q = mask & *q;
        } else {
            void *q = &s->unk4->inner;
            u32 mask = ~1;
            ((typeof(s->unk4->inner) *)q)->unk8 = mask & ((typeof(s->unk4->inner) *)q)->unk8;
        }
        {
            u16 one = 1;
            s->unk9C = one;
        }
        s->unk178 = 60;
    }
    if (!*((volatile u8 *)&s->unk2B0 + 3)) return;
    {
        sub_080B1E60(s);
        s->unk2CC = s->unk118;
        s->unk2D0 = s->unk11C;
        s->unk59 = s->unk120;
        s->unk2A8 = s->unk121;
        s->unk2A9 = s->unk122;
        s->unk2AA = s->unk123;
        s->unk21C = s->unk124;
        s->unk2B5 = s->unk128;
        s->unk2C0 = s->unk12E;
        s->unk2EA = s->unk12C;
        n = s->unkB0;
        if (n <= 0) n = 0;
        s->unk9C = n;
        s->unk2D6 = 0;
        s->unk2D4 = 0;
        s->unk2B0 = 0;
        s->unk2B1 = 1;
        s->unk2B2 = 1;
        s->unk2B3 = 0;
        s->unk304 = 0;
        s->unk2B6 = 0;
        s->unk2C2 = 64;
        s->unk2D8 = 0;
        s->unk2DC = 0;
        s->unk2E0 = 0;
        s->unk2E6 = 0;
        s->unk3CC = 0;
        s->unkDC = 0;
        sub_08072188(&s->unkDC);
    }
}
