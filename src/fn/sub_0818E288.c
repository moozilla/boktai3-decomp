#include "global.h"
struct FlashBody {
    u8 pad0[0x5BC];
    u16 palette;
    u8 pad5BE[6];
    u8 count;
};
struct FlashVec {
    u32 x : 16;
    s32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
struct FlashPart {
    u32 flags;
    u8 pad4[8];
    void *sprite;
    u8 pad10[0x3e];
    u16 flags4E;
    u8 pad50[0x1c];
    struct FlashVec pos;
    u8 pad74[0x2e];
    u16 flagsA2;
    u8 padA4[0x7a];
    u8 byte11E;
    u8 byte11F;
    u8 pad120[0xc];
    u8 counter12C;
    u8 byte12D;
    u8 byte12E;
    u8 pad12F[0x2d];
    struct FlashBody *parent;
    u8 pad160[0x70];
    void (*callback)(struct FlashPart *);
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
u8 Sprite_BlinkUntilExpired(void *, s32, s32, s32);
void AnimData_SelectPaletteBank(void *, u32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void Sound_PlaySongChecked(u32);
void Armor_UpdatePartFlash(struct FlashPart *p)
{
    struct FlashBody *parent = p->parent;
    if (reset((u8 *)p)) {
        p->flags4E |= 4;
        p->flagsA2 |= 4;
        p->counter12C = 0;
        p->byte12D = 0x14;
        p->callback = 0;
    }
    {
        u8 *counter = &p->counter12C;
        if (Sprite_BlinkUntilExpired(p, (*counter)++, 10, 30)) {
            u8 five;
            u8 one;
            AnimData_SelectPaletteBank(p->sprite, parent->palette);
            five = 5;
            p->byte11E = 0;
            p->byte11F = five;
            one = 1;
            *((u8 *)p + 0x119) = one | *((u8 *)p + 0x119);
            parent->count--;
        }
        if ((*counter & 7) == 0) {
            struct FlashVec a;
            struct FlashVec b;
            a = p->pos;
            a.x += 100;
            a.y += 100;
            a.z += 100;
            b.x = 0x50;
            b.y = 0x3C;
            b.z = 0x28;
            SparkParticle_Spawn(&a, 1, &b, 0, 0, 14, 2, 2);
            Sound_PlaySongChecked(0x16E);
        }
    }
}
