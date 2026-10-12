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
void sub_080B1E60(void *);
void sub_08072188(void *);
static inline u8 take_flag(u8 *s)
{
    if ((*(u8 *)(s + 0x2B2))) { (*(u8 *)(s + 0x2B2)) = 0; return 1; }
    return 0;
}

void sub_080AD270(u8 *s)
{
    s32 n;
    if (take_flag(s)) {
        sub_08013424(s + 0x320);
        if (!(*(u8 *)(s + 0x8))) (*(Owner * *)(s + 0x4))->unk0 &= ~1;
        else (*(Owner * *)(s + 0x4))->inner.unk8 &= ~1;
        (*(u16 *)(s + 0x9C)) = 1;
        (*(u16 *)(s + 0x178)) = 60;
    }
    if ((*(u8 *)(s + 0x2B3))) {
        sub_080B1E60(s);
        (*(u32 *)(s + 0x2CC)) = (*(u32 *)(s + 0x118));
        (*(u32 *)(s + 0x2D0)) = (*(u32 *)(s + 0x11C));
        (*(u8 *)(s + 0x59)) = (*(u8 *)(s + 0x120));
        (*(u8 *)(s + 0x2A8)) = (*(u8 *)(s + 0x121));
        (*(u8 *)(s + 0x2A9)) = (*(u8 *)(s + 0x122));
        (*(u8 *)(s + 0x2AA)) = (*(u8 *)(s + 0x123));
        (*(u32 *)(s + 0x21C)) = (*(u32 *)(s + 0x124));
        (*(u8 *)(s + 0x2B5)) = (*(u8 *)(s + 0x128));
        (*(u16 *)(s + 0x2C0)) = (*(u16 *)(s + 0x12E));
        (*(u16 *)(s + 0x2EA)) = (*(u16 *)(s + 0x12C));
        n = (*(s16 *)(s + 0xB0));
        if (n <= 0) n = 0;
        (*(u16 *)(s + 0x9C)) = n;
        (*(u16 *)(s + 0x2D6)) = 0;
        (*(u16 *)(s + 0x2D4)) = 0;
        (*(u8 *)(s + 0x2B0)) = 0;
        (*(u8 *)(s + 0x2B1)) = 1;
        (*(u8 *)(s + 0x2B2)) = 1;
        (*(u8 *)(s + 0x2B3)) = 0;
        (*(u32 *)(s + 0x304)) = 0;
        (*(u8 *)(s + 0x2B6)) = 0;
        (*(u16 *)(s + 0x2C2)) = 64;
        (*(u32 *)(s + 0x2D8)) = 0;
        (*(u32 *)(s + 0x2DC)) = 0;
        (*(u32 *)(s + 0x2E0)) = 0;
        (*(u16 *)(s + 0x2E6)) = 0;
        (*(u8 *)(s + 0x3CC)) = 0;
        (*(u32 *)(s + 0xDC)) = 0;
        sub_08072188(&(*(u32 *)(s + 0xDC)));
    }
}
