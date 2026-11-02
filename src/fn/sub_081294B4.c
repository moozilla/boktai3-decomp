#include "global.h"
#include "display_sequence.h"
#include "ram.h"
#include "engine_globals.h"
struct Vec {
    s16 x, y, z, pad;
};
struct FeatherSequence {
    const u16 *commands;
    u8 flags, frameCount, frameWeight, duration;
    u16 index, unknownA, speed, elapsed;
};
struct Feather {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[4];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct Vec position;
    u8 unknown24[8];
    struct FeatherSequence sequence;
    u8 unknown3C[0x54];
    u16 age;
    u8 state;
    s8 slot;
    u16 blinkAge;
    u8 enter, angle;
    u32 effectFlags, variant;
};
struct FeatherPool {
    u8 task[0x18];
    u8 graphics[0x1c];
    void *sequences;
    u32 used;
};
void Sprite_Remove(void *);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Anim_SetSequence(void *, void *, u32, u32, u32);
static inline void SelectDirection(u32 angle, u8 *row, u8 *flip)
{
    u8 quadrant = ((((s32)(angle >> 24)) + 0x60) >> 6) & 3;
    if (quadrant > 1) {
        *flip = 1;
        *row = 3 - quadrant;
    } else {
        *flip = 0;
        *row = quadrant;
    }
}

extern u32 gUnk_02000560;
struct FeatherPool *FeatherEffectPool_GetOrCreate(void);
struct Feather *FeatherEffectPool_AllocEntry(struct FeatherPool *);
static inline u32 FeatherBlockedSlots(u32 mask)
{
    u32 bits = gUnk_02000560;
    bits &= mask;
    return bits;
}
static inline u32 FeatherCreationBlockFlags(void)
{
    u32 mask = 0x400;
    u32 blocked = (gEngineFlags | gScenePauseFlags) & mask;
    if (blocked)
        return mask;
    blocked = FeatherBlockedSlots(4);
    return blocked;
}
// confidence=low: creates a directional feather effect with palette and scatter variants.
struct Feather *FeatherEffectPool_CreateDirectional(struct Vec *position, u32 angle, u32 flags,
                                                    u32 variant)
{
    struct FeatherPool *pool = gFadingSpritePool;
    struct Feather *p;
    struct Feather *sprite;
    u8 row, flip;
    if (!pool) {
        pool = FeatherEffectPool_GetOrCreate();
        if (!pool)
            return 0;
    }
    if (FeatherCreationBlockFlags())
        return 0;
    p = FeatherEffectPool_AllocEntry(pool);
    if (!p)
        return 0;
    p->age = 0;
    p->angle = angle;
    p->enter = 1;
    p->state = 2;
    sprite = p;
    sprite->position = *position;
    SelectDirection(angle << 24, &row, &flip);
    Sprite_Init(sprite, pool->graphics, 0);
    Anim_SetSequence(&p->sequence, pool->sequences, 1, row, flip);
    p->variant = variant;
    if (!variant) {
        AnimData_SelectPaletteBank(sprite->graphics, 0x278);
        p->effectFlags = 0x200004;
    } else {
        AnimData_SelectPaletteBank(sprite->graphics, 0x277);
        p->effectFlags = 0x400008;
    }
    {
        struct FeatherSequence *a = &p->sequence;
        const volatile u16 *entry;
        {
            u32 offset = a->index * 2;
            const u16 *table = p->sequence.commands;
            entry = (const volatile u16 *)((u32)table + offset);
        }
        sprite->frame = *entry >> 6;
        {
            union DisplaySequenceControl control;
            u32 bit, first;
            control.raw = a->flags;
            bit = 1;
            first = (u8)(control.raw & bit);
            if (first != (((*entry & 0x30) >> 4) & bit))
                sprite->flags |= 4;
            else
                sprite->flags &= ~4;
        }
        {
            union DisplaySequenceControl control;
            u32 bit, first;
            control.raw = a->flags;
            bit = 2;
            first = (u8)(control.raw & bit);
            if (first != (((*entry & 0x30) >> 4) & bit))
                sprite->flags |= 8;
            else
                sprite->flags &= ~8;
        }
        if (++a->elapsed >= a->duration) {
            a->elapsed = 0;
            if (a->flags & 4) {
                if (a->index == 0) {
                    if (a->flags & 8)
                        a->elapsed = a->duration;
                    else
                        a->index = a->frameCount - 1;
                } else
                    --a->index;
            } else {
                if (++a->index >= a->frameCount) {
                    if ((u8)(a->flags & 8)) {
                        a->elapsed = a->duration;
                        a->index = a->frameCount - 1;
                    } else
                        a->index = 0;
                }
            }
            entry = a->commands + a->index;
            a->frameWeight = *entry & 15;
            a->duration = (a->frameWeight * a->speed) >> 6;
            if (!a->duration)
                a->duration = 1;
        }
    }
    {
        u32 combined = sprite->flags | 1;
        sprite->priority = 1;
        combined |= flags;
        sprite->flags = combined;
    }
    return p;
}
