#include "bosses/vanargand/vanargand_pair_attack.h"
#include "scene/actor/motion_track.h"
#include "bosses/vanargand/paired_scene_actor.h"

// confidence=low: coordinates paired animations with an offset motion track.
void Vanargand_SetPendingCue(void *, u16);
s32 Vanargand_GetComponentWithFlag10(u8 *);
struct OrbitMotion;
s32 MotionTrack_Update(struct OrbitMotion *);
static inline u8 IsComplete(struct VanargandPairAttack *p)
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
void Vanargand_UpdateMovingPairAttack(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *self = p;
    u32 mask = 0x40;
    s32 result;
    s32 phase;
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 0);
    result = p->callback(p);
    if ((s16)result > 0)
        return;
    if (p->pending) {
        p->pending = result;
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
                    AddReferenceOffset(&p->coordinates.position, &destination);
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
