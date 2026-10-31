#include "global.h"
#include "engine_globals.h"
#include "ram.h"

// confidence=low: runs a seven-phase attack sequence for the paired actors: cues, animates the pair, and spawns projectiles.

struct VanargandHalf {
    u8 unk_00[0xB4];
    u8 collider[0x20C - 0xB4];
    s16 counter;
    u8 unk_20E[0x21C - 0x20E];
};
struct VanargandPair {
    struct VanargandHalf half[2];
};
struct Vanargand {
    u8 pad0[0x54];
    s16 pos[3];
    u8 pad5A[0x9C - 0x5A];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 padA8[7];
    u8 finished;
    u8 entered;
    u8 pending;
    u8 unkB2;
    u8 padB3[0xCC - 0xB3];
    u8 unkCC;
    u8 padCD[0x104 - 0xCD];
    void *callback;
    u8 pad108[0x110 - 0x108];
    void *target;
    u8 pad114[0x55D - 0x114];
    u8 unk55D;
    u8 pad55E[0x56C - 0x55E];
    struct VanargandPair *pair;
    u8 pad570[0x57C - 0x570];
    void *sprite;
    u8 pad580[0x5B0 - 0x580];
    u16 unk5B0;
    u8 pad5B2[0x5B4 - 0x5B2];
    s16 repeats;
    u8 pad5B6[0x5C7 - 0x5B6];
    u8 level;
};
void Vanargand_SetPendingCue(void *, u16);
s32 Callback_InvokeActor(void *, void *);
void PairedSceneActor_ApplyToSelectedHalves(void *, s32, s32);
s32 PairedSceneActor_AreSelectedHalvesFinished(void *);
s32 ScenePair_CountPositiveCounters(void *);
void MotionTrack_Initialize(void *, void *, void *, s32, s32);
void MotionTrack_Update(void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void Anim_SetSequence(void *, void *, u32, u32, u32);
void Sprite_ApplyAnimation(void *, void *);
void Sound_PlaySong(u32);
void sub_081B71D4(void *, void *, void *, void *, s32, s32, s32);
void VanargandProjectile_Create(void *, void *, void *, s32, s32, s32);
s32 sub_08249350(s32, s32);
struct Position {
    s16 x, y, z, pad;
};
struct HalfOrder {
    u32 index[2];
};
extern const struct HalfOrder gUnk_0824F788;
static inline void InitializeHalfOrder(struct HalfOrder *order)
{
    *order = gUnk_0824F788;
}

static inline s32 RandomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)gRandomTable);
}
static inline s32 RandomValueWith(const u16 *table)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)table);
}
static inline s32 SpanHalf(s32 sum)
{
    s32 value = (sum << 5) / 3 + 0x40;
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}
static inline u8 MotionComplete(struct Vanargand *p)
{
    u8 result = 0;
    if (p->unk55D == 2)
        result = 1;
    return result;
}
static inline void SetFlags(struct Vanargand *p, u32 mask)
{
    p->flags |= mask;
}
static inline void SetPosition(s16 *origin, s32 x, s32 y, s32 z)
{
    s16 *q = origin;
    *q = x;
    origin[1] = y;
    origin[2] = z;
}
struct PartAnimation {
    u8 unknown0[0x48];
    u8 animation[0x10];
    void *resource;
    u32 cursor;
};
static inline s32 RandomOffset(s32 base)
{
    s32 r = RandomValue() >> 3;
    s32 remainder = r % 8;
    return remainder + base;
}
static inline s32 RandomSpread(u8 base)
{
    const u16 *table = gRandomTable;
    s32 r = RandomValueWith(table) >> 3;
    s32 remainder = r % 32;
    remainder -= 0x10;
    return base + remainder;
}
static inline s32 RandomIndex(void)
{
    s32 v = RandomValue();
    return sub_08249350(v >> 3, 6);
}
static inline void SetPhase(struct Vanargand *p, u32 phase)
{
    u32 next = phase;
    u32 zero = 0;
    p->phase = next;
    p->timer = zero;
}
static inline void SpawnPartProjectile(struct Vanargand *owner, s16 *position, void *target,
                                       void *collider, u8 base, s32 sideOffset)
{
    const u16 *table = gRandomTable;
    s32 r = RandomValueWith(table) >> 3;
    s32 spread = r % 32;
    s32 index;
    spread -= 0x10;
    spread += base;
    index = RandomIndex();
    sub_081B71D4(owner->sprite, position, target, collider, spread, sideOffset, index);
}
static inline void SpawnFreeProjectile(struct Vanargand *owner, s16 *position, void *target,
                                       u8 base, s32 sideOffset)
{
    const u16 *table = gRandomTable;
    s32 r = RandomValueWith(table) >> 3;
    s32 spread = r % 32;
    s32 index;
    spread -= 0x10;
    spread += base;
    index = RandomIndex();
    VanargandProjectile_Create(owner->sprite, position, target, spread, sideOffset, index);
}
#ifdef NONMATCHING
// fuzzy: 87.3%
#define Vanargand_UpdateSweepingProjectileAttack sub_081AA788
void Vanargand_UpdateSweepingProjectileAttack(struct Vanargand *p)
{
    struct Vanargand *self = p;
    u8 *object = (u8 *)p + 0x18C;

    s32 sideOffset;
    u8 result;

    SetFlags(p, 0x40);
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
                SetPhase(p, 1);
            }
            break;
        case 1:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, object, object + 0x60, 8, 2, 0);
                p->timer++;
                {
                    struct PartAnimation *part = (struct PartAnimation *)((u8 *)self + 0x3FC);
                    Anim_SetSequence((u8 *)self + 0x444, part->resource, 0, 0, 0);
                    *(u16 *)((u8 *)self + 0x500) = 1;
                    part->cursor = 0;
                }
            }
            if (p->unkB2) {
                SetPhase(p, 2);
            }
            break;
        case 2:
            SetFlags(p, 0x80000);
            if (p->timer <= 3)
                self->unk5B0 = 8;
            p->timer++;
            {
                u8 band = self->level;
                s32 mask = 0x10;
                if (band > 1)
                    mask = 8;
                if (p->timer & mask) {
                    SetPhase(p, 3);
                }
            }
            break;
        case 3:
            SetFlags(p, 0x180000);
            if (p->timer == 0) {
                u32 timer = p->timer;
                s16 backward[4];
                s16 origin[4];
                struct HalfOrder halves;
                s32 x;
                s32 y;
                s32 offset;
                s16 *span;
                u32 selected;
                struct VanargandPair *pair;
                struct Vanargand *poolOwner;

                s32 vx = p->pos[0];
                s32 vy = p->pos[1];
                s32 vz = p->pos[2];
                SetPosition(backward, vx - 0x100, vy, vz - 0x100);
                MotionTrack_Initialize((u8 *)self + 0x534, p->pos, backward, 0, 1);
                SceneActors_StartUnmirroredAnimation(p, object, object + 0x60, 9, 2, 0);
                span = (s16 *)(object + 0x128);
                *(u32 *)origin = timer;
                x = origin[0];
                y = origin[1];
                span[0] = SpanHalf(x + y);
                span[1] = 0x40;
                span[2] = SpanHalf(y - x);
                poolOwner = p;
                InitializeHalfOrder(&halves);
                if (p->level > 1)
                    offset = RandomOffset(0x14);
                else
                    offset = RandomOffset(0xC);
                if (ScenePair_CountPositiveCounters(p->pair) == 0)
                    offset += 8;
                sideOffset = offset;

                selected = p->repeats & 1;
                pair = p->pair;
                selected = halves.index[selected];
                if (pair->half[selected].counter > 0) {
                    void *target = p->target;
                    void *collider = (void *)(selected * sizeof(struct VanargandHalf) +
                                              (u32)pair->half[0].collider);
                    SpawnPartProjectile(poolOwner, p->pos, target, collider, p->unkCC, sideOffset);
                } else {
                    selected = halves.index[(p->repeats + 1) & 1];
                    if (pair->half[selected].counter > 0) {
                        void *target = p->target;
                        void *collider = (void *)(selected * sizeof(struct VanargandHalf) +
                                                  (u32)pair->half[0].collider);
                        SpawnPartProjectile(poolOwner, p->pos, target, collider, p->unkCC,
                                            sideOffset);
                    } else {
                        void *target = p->target;
                        SpawnFreeProjectile(poolOwner, p->pos, target, p->unkCC, sideOffset);
                    }
                }
                Sound_PlaySong(0x583);
                p->timer++;
            }
            {
                s32 level = p->unkCC;
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
            MotionTrack_Update((u8 *)self + 0x534);
            p->timer++;
            if (MotionComplete(self)) {
                self->repeats++;
                p->phase = 4;
                p->timer = 0;
            }
            break;
        case 4:
            SetFlags(p, 0x180000);
            if (p->timer == 0) {
                struct Position forward;
                s32 vx = p->pos[0];
                s32 vy = p->pos[1];
                s32 vz = p->pos[2];
                SetPosition((s16 *)&forward, vx + 0x100, vy, vz + 0x100);
                if (self->level > 1)
                    MotionTrack_Initialize((u8 *)self + 0x534, p->pos, &forward, 0, 2);
                else
                    MotionTrack_Initialize((u8 *)self + 0x534, p->pos, &forward, 0, 4);
                p->timer++;
            }
            MotionTrack_Update((u8 *)self + 0x534);
            if (MotionComplete(self)) {
                if (self->repeats > 3) {
                    SetPhase(p, 5);
                } else {
                    SetPhase(p, 2);
                }
            }
            break;
        case 5:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, object, object + 0x60, 8, 2, 2);
                p->timer++;
                {
                    struct PartAnimation *part = (struct PartAnimation *)((u8 *)self + 0x3FC);
                    Anim_SetSequence((u8 *)self + 0x444, part->resource, 0, 0, 4);
                    *(u16 *)((u8 *)self + 0x500) = 1;
                    part->cursor = 0;
                }
            }
            if (p->unkB2) {
                SetPhase(p, 6);
            }
            break;
        case 6:
            p->finished = 1;
            break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081AA788);
#endif
