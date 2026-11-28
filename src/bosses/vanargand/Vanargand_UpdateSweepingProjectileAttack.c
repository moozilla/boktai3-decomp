#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
#include "scene/actor/scene_actor_components.h"
#include "scene/actor/motion_track.h"
#include "engine/math/monster_particle_random.h"

// confidence=low: runs a seven-phase attack sequence for the paired actors: cues, animates the pair, and spawns projectiles.

static inline struct ContactPosition *HalfPosition(struct VanargandAttackPair *pair, u32 index)
{
    u32 offset = index * sizeof(struct PairedSceneHalf);
    u32 halfwords = offset / sizeof(s16);
    struct VanargandAttackPair *base = (struct VanargandAttackPair *)((halfwords << 1) + (u32)pair);
    return &base->halves[0].sprite.position;
}
void Vanargand_SetPendingCue(void *, u16);
s32 Callback_InvokeActor(void *, void *);
void MotionTrack_Update(void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void Anim_SetSequence(void *, const void *, u32, u32, u32);
void Sprite_ApplyAnimation(void *, void *);
void Sound_PlaySong(u32);
s32 VanargandProjectile_CreateWithSource(void *, void *, void *, void *, u32, u32, u32);
s32 VanargandProjectile_Create(void *, void *, void *, u32, u32, u32);
s32 sub_08249350(s32, s32);
struct HalfOrder {
    u32 index[2];
};
extern const struct HalfOrder gUnk_0824F788;

static inline s32 SpanHalf(s32 sum)
{
    s32 value = (sum << 5) / 3 + 0x40;
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline u8 MotionComplete(struct VanargandPairAttack *p)
{
    u8 result = 0;
    if (p->track.active == 2)
        result = 1;
    return result;
}
static inline void SetPosition(s16 *origin, s32 x, s32 y, s32 z)
{
    s16 *q = origin;
    *q = x;
    origin[1] = y;
    origin[2] = z;
}
static inline s32 RandomOffsetValue(void)
{
    s32 value;
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    value = gRandomIndex * 2;
    value = *(u16 *)(value + RANDOM_TABLE_ADDR);
    return value;
}
static inline s32 RandomOffset(s32 base)
{
    s32 r = RandomOffsetValue() >> 3;
    s32 remainder = r % 8;
    return remainder + base;
}
static inline void SetPhasePointer(u8 *phase, u32 *timer, u32 value)
{
    *phase = value;
    *timer = 0;
}
static inline void SpawnPartProjectile(struct VanargandPairAttack *owner, struct SceneVec *position,
                                       void *target, struct ContactPosition *source, u16 base,
                                       s32 sideOffset)
{
    const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;
    u32 *cursor = &gRandomIndex;
    s32 r;
    s32 spread;
    s32 index;
    s32 negativeSpread;
    u32 sampleOffset;
    *cursor = (*cursor + 1) & 1023;
    PARTICLE_RANDOM_READ_SAMPLE(r, table, *cursor);
    r >>= 3;
    spread = (s16)r;
    if (spread < 0)
        spread += 31;
    spread >>= 5;
    spread <<= 5;
    spread = r - spread;
    spread -= 0x10;
    negativeSpread = -spread;
    spread = base - negativeSpread;
    *cursor = (*cursor + 1) & 1023;
    sampleOffset = *cursor * 2;
    r = *(u16 *)(sampleOffset + (u32)table);
    index = sub_08249350(r >> 3, 6);
    VanargandProjectile_CreateWithSource(owner->projectilePool, position, target, source, spread,
                                         sideOffset, index);
}
static inline void SpawnFreeProjectile(struct VanargandPairAttack *owner, struct SceneVec *position,
                                       void *target, u16 base, s32 sideOffset)
{
    const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;
    u32 *cursor = &gRandomIndex;
    s32 r;
    s32 spread;
    s32 index;
    s32 negativeSpread;
    u32 sampleOffset;
    *cursor = (*cursor + 1) & 1023;
    PARTICLE_RANDOM_READ_SAMPLE(r, table, *cursor);
    r >>= 3;
    spread = (s16)r;
    if (spread < 0)
        spread += 31;
    spread >>= 5;
    spread <<= 5;
    spread = r - spread;
    spread -= 0x10;
    negativeSpread = -spread;
    spread = base - negativeSpread;
    *cursor = (*cursor + 1) & 1023;
    sampleOffset = *cursor * 2;
    r = *(u16 *)(sampleOffset + (u32)table);
    index = sub_08249350(r >> 3, 6);
    VanargandProjectile_Create(owner->projectilePool, position, target, spread, sideOffset, index);
}
void Vanargand_UpdateSweepingProjectileAttack(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *self = p;
    u8 *object = (u8 *)&p->animation;

    s32 sideOffset;
    u8 result;

    VanargandPairAttack_SetFlags(p, 0x40);
    Vanargand_SetPendingCue(p, 0);
    result = Callback_InvokeActor(p, p->callback);
    if (result)
        return;
    if (p->pending) {
        p->pending = 0;
        p->repeats = 0;
        PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 0x13);
        p->phase = 0;
        p->timer = 0;
    }
    switch (p->phase) {
        case 0:
            if (PairedSceneActor_AreSelectedHalvesFinished(self->pair)) {
                PairedSceneActor_ApplyToSelectedHalves(self->pair, 3, 2);
                VanargandPairAttack_EnterPhase(p, 1);
            }
            break;
        case 1:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, object, object + 0x60, 8, 2, 0);
                p->timer++;
                {
                    struct VanargandDamagePart *part = &self->part;
                    Anim_SetSequence(&self->part.sequence, part->sequenceResource, 0, 0, 0);
                    self->part.animation = 1;
                    part->sequenceId = 0;
                }
            }
            if (p->animationEvent) {
                VanargandPairAttack_EnterPhase(p, 2);
            }
            break;
        case 2:
            VanargandPairAttack_SetFlags(p, 0x80000);
            if (p->timer <= 3)
                self->delay = 8;
            p->timer++;
            {
                u8 band = self->pairLevel;
                s32 mask = 0x10;
                if (band > 1)
                    mask = 8;
                if (p->timer & mask) {
                    VanargandPairAttack_EnterPhase(p, 3);
                }
            }
            break;
        case 3:
            VanargandPairAttack_SetFlags(p, 0x180000);
            if (p->timer == 0) {
                s16 backward[4];
                s16 origin[4];
                struct HalfOrder halves;
                s32 x;
                s32 y;
                s16 *span;
                u32 selected;
                u32 *halfIndex;
                struct VanargandAttackPair *pair;
                struct VanargandPairAttack *poolOwner;

                SetPosition(backward, p->position.x - 0x100, p->position.y, p->position.z - 0x100);
                MotionTrack_Initialize(&self->track, &p->position,
                                       (const struct MotionTrackVector *)backward, 0, 1);
                SceneActors_StartUnmirroredAnimation(p, object, object + 0x60, 9, 2, 0);
                span = (s16 *)(object + 0x128);
                *(u32 *)origin = 0;
                x = origin[0];
                y = origin[1];
                span[0] = SpanHalf(x + y);
                span[1] = 0x40;
                span[2] = SpanHalf(-x + y);
                poolOwner = p;
                halves = gUnk_0824F788;
                if (p->pairLevel > 1)
                    sideOffset = RandomOffset(0x14);
                else
                    sideOffset = RandomOffset(0xC);
                if (ScenePair_CountPositiveCounters(poolOwner->pair) == 0)
                    sideOffset += 8;

                halfIndex = &halves.index[poolOwner->repeats & 1];
                pair = poolOwner->pair;
                selected = *halfIndex;
                if (pair->halves[selected].health > 0) {
                    void *target = p->destination;
                    struct ContactPosition *source = HalfPosition(pair, selected);
                    SpawnPartProjectile(poolOwner, &p->position, target, source,
                                        p->targetBearings[0], sideOffset);
                } else {
                    u32 alternate = halves.index[(poolOwner->repeats + 1) & 1];
                    if (pair->halves[alternate].health > 0) {
                        void *target = p->destination;
                        struct ContactPosition *source = HalfPosition(pair, alternate);
                        SpawnPartProjectile(poolOwner, &p->position, target, source,
                                            p->targetBearings[0], sideOffset);
                    } else {
                        void *target = p->destination;
                        SpawnFreeProjectile(poolOwner, &p->position, target, p->targetBearings[0],
                                            sideOffset);
                    }
                }
                Sound_PlaySong(0x583);
                p->timer++;
            }
            {
                s32 level = p->targetBearings[0];
                u8 *sprite = object + 0xA8;
                s32 anim;

                if (level <= 0x10)
                    anim = 0x17;
                else if (level <= 0x2F)
                    anim = 0x18;
                else if (level <= 0x40)
                    anim = 0x19;
                else {
                    anim = 0x16;
                    if (level <= 0x9F)
                        anim = 0x1A;
                }
                Sprite_ApplyAnimation(sprite, sprite + 0x2C);
                *(u16 *)(sprite + 0x10) = anim;
            }
            MotionTrack_Update(&self->track);
            p->timer++;
            if (MotionComplete(self)) {
                s16 *repeats = &self->repeats;
                ++*repeats;
                SetPhasePointer(&p->phase, &p->timer, 4);
            }
            break;
        case 4:
            VanargandPairAttack_SetFlags(p, 0x180000);
            if (p->timer == 0) {
                struct ContactPosition forward;
                SetPosition((s16 *)&forward, p->position.x + 0x100, p->position.y,
                            p->position.z + 0x100);
                if (self->pairLevel > 1)
                    MotionTrack_Initialize(&self->track, &p->position,
                                           (const struct MotionTrackVector *)&forward, 0, 2);
                else
                    MotionTrack_Initialize(&self->track, &p->position,
                                           (const struct MotionTrackVector *)&forward, 0, 4);
                p->timer++;
            }
            MotionTrack_Update(&self->track);
            if (MotionComplete(self)) {
                if (self->repeats > 3) {
                    VanargandPairAttack_EnterPhase(p, 5);
                } else {
                    VanargandPairAttack_EnterPhase(p, 2);
                }
            }
            break;
        case 5:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, object, object + 0x60, 8, 2, 2);
                p->timer++;
                {
                    struct VanargandDamagePart *part = &self->part;
                    u32 flags = 4;
                    Anim_SetSequence(&self->part.sequence, part->sequenceResource, 0, 0, flags);
                    self->part.animation = 1;
                    part->sequenceId = 0;
                }
            }
            if (p->animationEvent) {
                VanargandPairAttack_EnterPhase(p, 6);
            }
            break;
        case 6:
            p->finished = 1;
            break;
    }
}
