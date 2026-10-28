#include "global.h"
#include "engine_globals.h"
#include "ram.h"

// confidence=low: runs a seven-phase attack sequence for the paired actors: cues, animates the pair, and spawns projectiles.

// confidence=low: runs a seven-phase attack sequence for the paired actors: cues, animates the pair, and spawns projectiles.

// confidence=low: runs a seven-phase attack sequence for the paired actors: cues, animates the pair, and spawns projectiles.
struct VanargandHalf {
    u8 unk_00[0x20C];
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
extern const u32 gUnk_0824F788[2];

static inline u16 RandomValue(void)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    return gRandomTable[gRandomIndex];
}
static inline s16 SpanHalf(s32 sum)
{
    return ((sum << 5) / 3 + 0x40) / 2;
}
static inline s32 RandomOffset(s32 base)
{
    s32 r = RandomValue() >> 3;
    return (r - ((r >> 3) << 3)) + base;
}
static inline s32 RandomSpread(u8 base)
{
    s32 r = RandomValue() >> 3;
    return base + (r - ((r >> 5) << 5)) - 0x10;
}
static inline s32 RandomIndex(void)
{
    u16 v = RandomValue();
    return sub_08249350(v >> 3, 6);
}
#ifdef NONMATCHING
// fuzzy: 29.1%
// scope shape 453 (differences once low registers are set aside)
// confidence=low: runs a seven-phase attack sequence for the paired actors: cues, animates the pair, and spawns projectiles.
void sub_081AA788(struct Vanargand *p)
{
    u8 *object = (u8 *)p + 0x18C;
    u8 *motion = (u8 *)p + 0x534;
    u32 halves[2];
    s32 sideOffset;
    u8 result;

    p->flags |= 0x40;
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
            if (PairedSceneActor_AreSelectedHalvesFinished(p->pair)) {
                PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 2);
                p->phase = 1;
                p->timer = 0;
            }
            break;
        case 1:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, object, object + 0x60, 8, 2, 0);
                p->timer++;
                Anim_SetSequence((u8 *)p + 0x444, *(void **)((u8 *)p + 0x454), 0, 0, 0);
                *(u16 *)((u8 *)p + 0x500) = 1;
                *(u32 *)((u8 *)p + 0x458) = 0;
            }
            if (p->unkB2) {
                p->phase = 2;
                p->timer = 0;
            }
            break;
        case 2:
            p->flags |= 0x80000;
            if (p->timer <= 3)
                p->unk5B0 = 8;
            p->timer++;
            if (p->timer & (p->level > 1 ? 8 : 0x10)) {
                p->phase = 3;
                p->timer = 0;
            }
            break;
        case 3:
            p->flags |= 0x180000;
            if (p->timer == 0) {
                s16 vec[3];
                union {
                    u32 word;
                    s16 half[2];
                } origin;
                s32 x;
                s32 y;
                s32 offset;
                s32 spread;
                s32 index;
                s16 *span;
                struct VanargandHalf *half;

                vec[0] = p->pos[0] - 0x100;
                vec[1] = p->pos[1];
                vec[2] = p->pos[2] - 0x100;
                MotionTrack_Initialize(motion, p->pos, vec, 0, 1);
                SceneActors_StartUnmirroredAnimation(p, object, object + 0x60, 9, 2, 0);
                origin.word = p->timer;
                x = origin.half[0];
                y = origin.half[1];
                span = (s16 *)(object + 0x128);
                span[0] = SpanHalf(x + y);
                span[1] = 0x40;
                span[2] = SpanHalf(y - x);
                halves[0] = gUnk_0824F788[0];
                halves[1] = gUnk_0824F788[1];
                if (p->level > 1)
                    offset = RandomOffset(0x14);
                else
                    offset = RandomOffset(0xC);
                if (ScenePair_CountPositiveCounters(p->pair) == 0)
                    offset += 8;
                sideOffset = offset;

                half = &p->pair->half[halves[p->repeats & 1]];
                if (half->counter > 0) {
                    spread = RandomSpread(p->unkCC);
                    index = RandomIndex();
                    sub_081B71D4(p->sprite, p->pos, p->target, (u8 *)half + 0xB4, spread,
                                 sideOffset, index);
                } else {
                    half = &p->pair->half[halves[(p->repeats + 1) & 1]];
                    if (half->counter > 0) {
                        spread = RandomSpread(p->unkCC);
                        index = RandomIndex();
                        sub_081B71D4(p->sprite, p->pos, p->target, (u8 *)half + 0xB4, spread,
                                     sideOffset, index);
                    } else {
                        spread = RandomSpread(p->unkCC);
                        index = RandomIndex();
                        VanargandProjectile_Create(p->sprite, p->pos, p->target, spread, sideOffset,
                                                   index);
                    }
                }
                Sound_PlaySong(0x583);
                p->timer++;
            }
            {
                s32 anim;
                u8 level = p->unkCC;

                if (level <= 0x10)
                    anim = 0x17;
                else if (level <= 0x2F)
                    anim = 0x18;
                else if (level <= 0x40)
                    anim = 0x19;
                else if (level > 0x9F)
                    anim = 0x16;
                else
                    anim = 0x1A;
                Sprite_ApplyAnimation(object + 0xA8, object + 0xD4);
                *(u16 *)(object + 0xB8) = anim;
            }
            MotionTrack_Update(motion);
            p->timer++;
            if (p->unk55D == 2) {
                p->repeats++;
                p->phase = 4;
                p->timer = 0;
            }
            break;
        case 4:
            p->flags |= 0x180000;
            if (p->timer == 0) {
                s16 vec[3];

                vec[0] = p->pos[0] + 0x100;
                vec[1] = p->pos[1];
                vec[2] = p->pos[2] + 0x100;
                if (p->level > 1)
                    MotionTrack_Initialize(motion, p->pos, vec, 0, 2);
                else
                    MotionTrack_Initialize(motion, p->pos, vec, 0, 4);
                p->timer++;
            }
            MotionTrack_Update(motion);
            if (p->unk55D == 2) {
                p->phase = (p->repeats <= 3) ? 2 : 5;
                p->timer = 0;
            }
            break;
        case 5:
            if (p->timer == 0) {
                SceneActors_StartUnmirroredAnimation(p, object, object + 0x60, 8, 2, 2);
                p->timer++;
                Anim_SetSequence((u8 *)p + 0x444, *(void **)((u8 *)p + 0x454), 0, 0, 4);
                *(u16 *)((u8 *)p + 0x500) = 1;
                *(u32 *)((u8 *)p + 0x458) = 0;
            }
            if (p->unkB2) {
                p->phase = 6;
                p->timer = 0;
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
