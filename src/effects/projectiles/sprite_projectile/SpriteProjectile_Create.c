#include "engine/word_flags.h"
#include "effects/projectiles/sprite_projectile.h"
#include "global.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "engine/video/display_sequence_inline.h"
#include "engine/math/monster_particle_random.h"
#include "engine/engine_pause.h"
// confidence=low: creates a pooled projectile with optional spin and collider.
void *SpriteProjectile_EnsurePool(void);
struct Projectile *SpriteProjectilePool_AllocateSlot(void *);
extern u32 gUnk_02000560;
void AnimData_Load(void *, u32);
void *ScriptProc_Start(u32, u16);
void Sprite_Init(void *, void *, s32);
void Anim_SetSequence(void *, void *, u16, u8, u8);
void AnimData_SelectPaletteBank(const void *, s32);
void sub_081274F4(void *, s32, s32, s32, s32);
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
    base->sprite.frame = (*command) >> 6;
    {
        union DisplaySequenceControl control;
        u32 first;
        control.raw = state->flags;
        bit = 1;

        first = (u8)(control.raw & bit);
        if (first != ((((*command) & 0x30) >> 4) & bit))
            base->sprite.flags |= 4;
        else
            base->sprite.flags &= ~4;
    }
    {
        union DisplaySequenceControl control;
        u32 bit;
        u32 first;
        control.raw = state->flags;
        bit = 2;

        first = (u8)(control.raw & bit);

        if (first != ((((*command) & 0x30) >> 4) & bit))
            sprite->sprite.flags |= 8;
        else
            sprite->sprite.flags &= ~8;
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

struct Projectile *SpriteProjectile_Create(struct ContactPosition *position,
                                           struct ContactPosition *velocity, u32 resource,
                                           u32 script, u32 sequence, u32 lifetime, u32 flags,
                                           u32 colliderFlags, s32 palette, u32 flip, u32 speed,
                                           s32 colliderKind, s32 sizeX, s32 sizeY, s32 sizeZ)
{
    struct ProjectilePool *pool = gPooledSpriteManager;
    struct Projectile *p;
    struct Projectile *sprite;
    union {
        struct DisplaySequenceState *sequence;
        u32 active;
    } playback;
    if (!pool) {
        pool = SpriteProjectile_EnsurePool();
        if (!pool)
            return 0;
    }
    if (Engine_PauseBits(0x400))
        return 0;
    if (Flags_MaskedWord(4, &gUnk_02000560))
        return 0;
    p = SpriteProjectilePool_AllocateSlot(pool);
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
    p->sprite.position = *position;
    AnimData_Load(&p->graphics, resource);
    p->sequenceResource = ScriptProc_Start(0x922E, script);
    Sprite_Init(&p->sprite, &p->graphics, 0);
    playback.sequence = &p->sequence;
    Anim_SetSequence(playback.sequence, p->sequenceResource, sequence, flip, speed);
    AnimData_SelectPaletteBank(p->sprite.resource, palette);
    SpriteProjectile_StepPlayback(sprite, playback.sequence, p);
    {
        u32 attributes = sprite->sprite.flags;
        playback.active = 1;
        attributes |= playback.active;
        sprite->sprite.priority = playback.active;
        attributes |= flags;
        sprite->sprite.flags = attributes;
    }
    if (flags & 2) {
        u32 origin;
        const u16 *table;
        s32 spinSample;
        PARTICLE_RANDOM_CAPTURE_ORIGIN(origin);
        table = (const u16 *)RANDOM_TABLE_ADDR;
        sprite->sprite.attribute = ParticleRandom_ReadDirectSample(table);
        spinSample = ParticleRandom_ReadDirectSample(table);
        spinSample &= 255;
        if (spinSample <= 127)
            p->spin = -1;
        else
            p->spin = playback.active;
        p->spinScale =
            (ParticleRandom_ReadRelativeSample(origin, (const u16 *)RANDOM_TABLE_ADDR) & 7) + 7;
    }
    if (colliderKind > 0) {
        sub_081274F4(p, colliderKind, sizeX, sizeY, sizeZ);
        p->colliderFlags |= 1;
    }
    return p;
}
