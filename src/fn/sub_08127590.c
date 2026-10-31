#include "global.h"
#include "ram.h"
#include "engine_globals.h"
#include "display_sequence_inline.h"
#include "heap.h"
#include "resource_registry.h"
#include "monster_particle_random.h"
// confidence=low: creates a pooled projectile with optional spin and collider.
#define SpriteProjectile_Create sub_08127590
#ifdef NONMATCHING
// fuzzy: 98.5%
struct ParticleVector {
    u16 x, y, z, unused;
};
struct Projectile {
    u32 flags;
    u8 unknown4[2], angle, priority;
    u8 unknown8[4];
    u32 paletteResource;
    u16 frame;
    u8 unknown12[10];
    struct ParticleVector position;
    u8 unknown24[8];
    struct DisplaySequenceState sequence;
    u8 graphics[0x1C];
    u8 collider[0x54];
    void *sequenceResource;
    struct ParticleVector velocity;
    u8 unknownB8;
    s8 spin;
    u8 spinScale, colliderFlags;
    u16 timer, state;
    u8 active;
    s8 slot;
    u8 unknownC2[2];
    u32 resourceId;
    u8 enter, soundDelay;
    u16 lifetime, cell;
};
struct ProjectilePool {
    u8 unknown0[0x860];
    u32 allocated;
};
void *SpriteProjectile_EnsurePool(void);
struct Projectile *SpritePool10_AllocateSlot(void *);
extern u32 gUnk_02000560;
void AnimData_Load(void *, u32);
void *ScriptProc_Start(u32, u16);
void Sprite_Init(void *, void *, s32);
void Anim_SetSequence(void *, void *, u16, u8, u8);
void AnimData_SelectPaletteBank(u32, s32);
void sub_081274F4(void *, s32, s32, s32, s32);
static inline u32 Busy(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}
static inline u32 TestFlags(u32 mask, u32 *value)
{
    return *value & mask;
}
static inline void SpriteProjectile_StepPlayback(struct Projectile *sprite,
                                                 struct DisplaySequenceState *state,
                                                 struct Projectile *base)
{
    const u16 *command;
    u32 flags;
    u32 bit;
    {
        u32 offset = state->index << 1;
        const u16 *table;
        table = base->sequence.commands;
        command = (const u16 *)(((u32)table) + offset);
    }
    base->frame = (*command) >> 6;
    {
        union DisplaySequenceControl control;
        u32 first;
        u32 maskBit = 1;
        control.raw = state->flags;
        bit = maskBit;

        first = (u8)(control.raw & bit);
        if (first != ((((*command) & 0x30) >> 4) & bit))
            base->flags |= 4;
        else
            base->flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit;
        u32 first;
        control.raw = state->flags;
        bit = 2;

        first = (u8)(control.raw & bit);

        if (first != ((((*command) & 0x30) >> 4) & bit))
            sprite->flags |= 8;
        else
            sprite->flags &= ~8;
    }
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        flags = state->flags;
        if (flags & 4) {
            if (state->index == 0) {
                u32 hold = 8;
                hold &= flags;
                if (hold)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
            } else {
                state->index--;
            }
        } else {
            if (++state->index >= state->frameCount) {
                u32 hold = 8;
                hold &= flags;
                if ((u8)hold) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
            }
        }
        {
            u32 offset = state->index << 1;
            const u16 *table;
            table = state->commands;
            command = (const u16 *)((u32)table + offset);
        }
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        {
            s32 weight = state->frameWeight;
            s32 speed = state->speed;
            speed *= weight;
            speed >>= 6;
            state->duration = speed;
            if (!(u8)speed)
                state->duration = 1;
        }
    }
}

struct Projectile *SpriteProjectile_Create(struct ParticleVector *position,
                                           struct ParticleVector *velocity, u32 resource,
                                           u32 script, u32 sequence, u32 lifetime, u32 flags,
                                           u32 colliderFlags, s32 palette, u32 flip, u32 speed,
                                           s32 colliderKind, s32 sizeX, s32 sizeY, s32 sizeZ)
{
    struct ProjectilePool *pool = gPooledSpriteManager;
    struct Projectile *p;
    struct Projectile *sprite;
    if (!pool) {
        pool = SpriteProjectile_EnsurePool();
        if (!pool)
            return 0;
    }
    if (Busy(0x400))
        return 0;
    if (TestFlags(4, &gUnk_02000560))
        return 0;
    p = SpritePool10_AllocateSlot(pool);
    if (!p)
        return 0;
    p->state = 0;
    p->enter = 1;
    p->velocity = *velocity;
    p->lifetime = lifetime;
    p->colliderFlags = colliderFlags;
    p->resourceId = resource;
    p->soundDelay = 0;
    if (!p->colliderFlags) {
        p->active = 1;

    } else {
        p->active = 2;
    }
    sprite = p;
    {
        struct ParticleVector *src = position;

        p->position = *src;
    }
    AnimData_Load(p->graphics, resource);
    p->sequenceResource = ScriptProc_Start(0x922E, script);
    Sprite_Init(p, p->graphics, 0);
    Anim_SetSequence(&p->sequence, p->sequenceResource, sequence, flip, speed);
    AnimData_SelectPaletteBank(p->paletteResource, palette);
    SpriteProjectile_StepPlayback(sprite, &p->sequence, p);
    {
        u32 attributes = sprite->flags;
        attributes |= 1;
        sprite->priority = 1;
        attributes |= flags;
        sprite->flags = attributes;
    }
    if (flags & 2) {
        u32 origin = (u32)&gHeapHead + RESOURCE_REGISTRY_ENTRIES_ADDR;
        const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;
        s32 spinSample;
        sprite->angle = ParticleRandom_ReadDirectSample(table);
        spinSample = ParticleRandom_ReadDirectSample(table);
        spinSample &= 255;
        if (spinSample <= 127)
            p->spin = -1;
        else
            p->spin = 1;
        p->spinScale =
            (ParticleRandom_ReadRelativeSample(origin, (const u16 *)RANDOM_TABLE_ADDR) & 7) + 7;
    }
    if (colliderKind > 0) {
        sub_081274F4(p, colliderKind, sizeX, sizeY, sizeZ);
        p->colliderFlags |= 1;
    }
    return p;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08127590);
#endif
