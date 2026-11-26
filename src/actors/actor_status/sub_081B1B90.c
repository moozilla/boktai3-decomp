#include "actors/paired_scene_actor/paired_scene_actor.h"
#include "engine/shared/global.h"
#include "engine/shared/global.h"
// confidence=low: provisional role inferred from state transitions and player-motion calls

// confidence=low: approaches a side of the player before playing sequence eight

#include "effects/shared_display_animation_wrapper/display_animation.h"
#include "engine/shared/scene_vec.h"
#include "player/player_cue/player_cue_sequence.h"
void MotionTrack_Update(void *);
struct SceneVec *Player_GetEffectivePosition(u32);
void OrbitMotion_BeginDistanceWave(void *, const void *, const void *, u32, u32);
static inline void Difference(struct SceneVec *out, struct SceneVec *a, struct SceneVec *b)
{
    out->x = (u16)a->x - (u16)b->x;
    out->y = (u16)a->y - (u16)b->y;
    out->z = (u16)a->z - (u16)b->z;
}
static inline u8 MotionEnded(struct PairedSceneHalf *p)
{
    u8 result = 0;
    if (p->motion.active == 2)
        result = 1;
    return result;
}
void ActorStatus_UpdateFlankingStrike(struct VanargandAttackPair *boss, struct PairedSceneHalf *p)
{
    struct SceneVec target;
    u8 *enter;
    u32 mask = 8;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        PairedSceneHalf_BeginActionPhase(p, 0);
    }
    switch (p->actionCounters[0]) {
        case 0: {
            u32 entered;
            {
                u8 *address = &p->actionEntered;
                entered = *address;
                enter = address;
            }
            if (entered) {
                u32 zero = 0;
                *enter = zero;
                Difference(&target, Player_GetEffectivePosition(zero),
                           (struct SceneVec *)p->origin);
                if (p->mode == 1) {
                    target.x += 512;
                    target.z -= 512;
                } else {
                    target.x -= 512;
                    target.z += 512;
                }
                OrbitMotion_BeginDistanceWave(&p->motion, &p->offset, &target,
                                              ((p->mode * 128) + 96) & 255, 4);
            }
            MotionTrack_Update(&p->motion);
            if (MotionEnded(p)) {
                u32 next = 1;
                p->actionCounters[0] = next;
                p->actionCounters[1] = 0;
                *enter = next;
            }
            break;
        }

        case 1: {
            struct DisplaySequenceState *seq;
            u32 entered;
            {
                u8 *address = &p->actionEntered;
                entered = *address;
                enter = address;
            }
            seq = &p->graphics.sequence;
            if (entered) {
                u32 zero = 0;
                *enter = zero;
                Anim_SetSequence(seq, (void *)boss->effectResource, 8, (u8)p->mode, zero);
            }
            if (PlayerCue_StepSequenceEnded(&p->sprite, seq)) {
                {
                    u32 next = 2;
                    p->actionCounters[0] = next;
                }
                p->actionCounters[1] = 0;
                *enter = 1;
            }
            break;
        }

        case 2:
            p->finished = 1;
            break;
    }
}
