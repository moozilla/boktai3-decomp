#include "global.h"
#include "engine.h"
// MATCHING_CONCESSION: register-pinning
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
s32 Div(s32, s32);
struct StretchBody {
    u8 pad0[0x5AC];
    u16 x, y, z;
    u8 pad5B2[0x6];
    u16 value;
    u8 pad5BA[0x9];
    u8 count;
    u8 pad5C4;
    u8 base;
};
struct StretchPart {
    u32 flags;
    u8 pad4[2];
    u8 rotation;
    u8 pad7;
    s8 byte8;
    s8 byte9;
    u8 padA[0x12];
    u16 x, y, z;
    u8 pad22[0x2c];
    u16 flags4E;
    u8 pad50[2];
    u16 param52;
    u8 pad54[0x4e];
    u16 flagsA2;
    u8 padA4[2];
    u16 paramA6;
    u8 padA8[0x62];
    u16 id10A;
    u8 pad10C[0xc];
    u8 index118;
    u8 flags119;
    u8 pad11A[0x42];
    struct StretchBody *parent;
    u8 pad160[0x70];
    void (*callback)(struct StretchPart *);
    u8 pad1D4[5];
    u8 angle;
    s8 spin;
};
static inline u8 reset(u8 *p)
{
    s32 one = 1;
    struct F {
        u8 flags;
    } *flag = (struct F *)(p + 0x119);
    u8 old = flag->flags;
    if (old & one) {
        s32 mask = ~1;
        s32 value = mask;
        s32 zero;
        value &= old;
        zero = 0;
        flag->flags = value;
        p[0x12e] = zero;
        return 1;
    }
    return 0;
}
void Armor_BeginPartStretch(struct StretchPart *);
void Armor_InitPartStretch(struct StretchPart *p)
{
    if (reset((u8 *)p)) {
        struct StretchBody *parent = p->parent;
        s32 one, choice;
        register s32 turn asm("r1");
        p->id10A = parent->value;
        p->angle = parent->base + Div(256, parent->count) * p->index118;
        p->x = parent->x + Div(gUnk_086149C4[(p->angle + 64) & 255] * 200, 4096);
        p->y = parent->y + 220;
        p->z = parent->z + Div(gUnk_086149C4[p->angle] * 200, 4096);
        p->flags &= ~1;
        p->rotation = 128;
        p->byte8 = 127;
        one = 1;
        p->byte9 = one;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 offset = gRandomIndex * 2;
            choice = *(const u16 *)(offset + (u32)gUnk_0203B400);
        }
        choice &= one;
        {
            register s32 negative asm("r2") = -1;
            asm("" : "+r"(negative));
            turn = negative;
        }
        if (choice)
            turn = 1;
        {
            register u32 off asm("r3") = 0x1DA;
            register u32 addr asm("r0");
            asm("" : "+r"(off));
            addr = (u32)p + off;
            *(s8 *)addr = turn;
        }
        p->flagsA2 &= ~4;
        p->flags4E &= ~4;
        p->paramA6 = 16;
        p->param52 = 16;
        {
            u32 off = 0x1D0;
            u32 addr;
            addr = (u32)p + off;
            *(void **)addr = Armor_BeginPartStretch;
        }
    }
}
