#include "global.h"

// foundry draft (worker best 97.8%); what differs is mostly register allocation and instruction order
struct EffectPoolSprite {
    u8 pad0[7];
    u8 mode;
    u8 pad8[8];
    u16 scale;
    u8 pad12[0x2c - 0x12];
};
struct EffectPoolAnimation {
    u8 pad0[6];
    u16 tile;
    u8 pad8[0x14];
};
struct EffectPoolSlot {
    s32 active;
    struct EffectPoolSprite sprite;
    struct EffectPoolAnimation animation;
};
s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_Init(void *, void *, u32);
static inline void CpuSetC(void *dst, u32 control, const void *src)
{
    CpuSet(src, dst, control);
}
void EffectPool_InitSlot(void *pool, struct EffectPoolSlot *slot, s32 index)
{
    vu32 zero;
    struct EffectPoolSprite *sprite;
    void *position;
    const void *source;
    slot->active = 0;
    AnimData_Load(&slot->animation, 0x8202);
    AnimData_SelectPaletteBank(&slot->animation, 0x6f);
    slot->animation.tile = 0x1db;
    sprite = &slot->sprite;
    Sprite_Init(sprite, &slot->animation, 1);
    sprite->scale = 8;
    sprite->mode = 1;
    position = (u8 *)slot + 0x20;
    zero = 0;
    source = (const void *)&zero;
    CpuSetC(position, 0x05000002, source);
}
