#include "motion_track.h"
#include "paired_scene_actor.h"

// confidence=low: coordinates paired animations with an offset motion track.
#define Vanargand_UpdateMovingPairAttack sub_081A93A0
struct MovingPairActor {
    u8 pad0[0x54];
    struct MotionTrackVector position;
    u8 pad5C[0x9C - 0x5C];
    u32 flags;
    u8 padA0[2];
    u8 phase, frame;
    u32 timer;
    u8 padA8[7];
    u8 finished, pending, pendingAnimation;
    u8 padB2[0x104 - 0xB2];
    u8 (*callback)(struct MovingPairActor *);
    u8 pad108[0x17C - 0x108];
    struct MotionTrackVector reference;
    u8 pad184[0x534 - 0x184];
    struct MotionTrack track;
    struct VanargandAttackPair *pair;
};
void Vanargand_SetPendingCue(void *, u16);
s32 Vanargand_GetComponentWithFlag10(u8 *);
struct OrbitMotion;
s32 MotionTrack_Update(struct OrbitMotion *);
static inline u8 IsComplete(struct MovingPairActor *p)
{
    u8 result = 0;
    if (p->track.active == 2)
        result = 1;
    return result;
}
static inline void SetVector(struct MotionTrackVector *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}
static inline void AddReferenceOffset(const struct MotionTrackVector *base,
                                      struct MotionTrackVector *v)
{
    v->x = v->x + base->x;
    v->y = base->y + v->y;
    v->z = v->z + base->z;
}
void Vanargand_UpdateMovingPairAttack(struct MovingPairActor *p)
{
    struct MovingPairActor *self = p;
    u32 mask = 0x40;
    s32 result;
    s32 phase;
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 0);
    result = p->callback(p);
    if ((s16)result > 0)
        return;
    if (p->pendingAnimation) {
        p->pendingAnimation = result;
        p->phase = result;
        p->timer = result;
    }
    phase = p->phase;
    switch (phase) {
        case 0:
            if (p->timer == 0) {
                if (!PairedSceneActor_ApplyToActiveHalf(p->pair, 13)) {
                    p->phase = 3;
                    p->timer = phase;
                }
                p->timer++;
            } else if (PairedSceneActor_AreSelectedHalvesFinished(p->pair)) {
                u32 mask = 4;
                p->flags |= mask;
                p->phase = 1;
                p->timer = phase;
            }
            break;

        case 1: {
            u32 mask = 4;
            p->flags |= mask;
            {
                u32 timer = p->timer;
                if (timer == 0) {
                    struct MotionTrackVector destination;
                    s32 part = Vanargand_GetComponentWithFlag10((u8 *)p->pair);
                    if (part == 0)
                        PairedSceneActor_ApplyToSelectedHalves(p->pair, 1, 14);
                    else if (part == 1)
                        PairedSceneActor_ApplyToSelectedHalves(p->pair, 2, 14);
                    else {
                        p->phase = 3;
                        p->timer = timer;
                    }
                    SetVector(&destination, 576, 512, 576);
                    AddReferenceOffset(&p->reference, &destination);
                    MotionTrack_Initialize(&p->track, &self->position, &destination, 0, 4);
                    p->timer++;
                } else if (PairedSceneActor_AreSelectedHalvesFinished(p->pair) && IsComplete(p)) {
                    p->phase = 2;
                    p->timer = result;
                } else {
                    MotionTrack_Update((struct OrbitMotion *)&p->track);
                }
            }
            break;
        }

        case 3:
            p->finished = 1;
            PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 2);
            break;

        case 2:
            p->finished = 1;
            {
                u32 mask = 0x20000;
                u32 flags = p->flags | mask;
                p->flags = flags | 4;
            }
            PairedSceneActor_ApplyToSelectedHalves(p->pair, 3, 19);
            break;
    }
}
