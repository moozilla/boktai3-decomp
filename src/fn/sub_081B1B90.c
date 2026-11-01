#include "global.h"
#include "global.h"
// confidence=low: provisional role inferred from state transitions and player-motion calls

// confidence=low: approaches a side of the player before playing sequence eight

#include "display_animation.h"
#include "scene_vec.h"
#include "player_cue_sequence.h"
struct ActorStatusPart {
    u32 spriteFlags;
    u8 unknown4[0xc];
    u16 frame;
    u8 unknown12[0xa];
    struct SceneVec position;
    u8 unknown24[0x24];
    struct DisplaySequenceState sequence;
    u8 motion[0x38];
    struct SceneVec *anchor;
    u8 unknown94[8];
    struct SceneVec offset;
    u8 unknownA4[0xb8];
    u32 variant, flags, state, timer;
    u8 action, previousAction, reset, enter, finished;
    u8 unknown171[0x2b];
    u32 countdown;
};
void MotionTrack_Update(void *);
struct SceneVec *Player_GetEffectivePosition(u32);
void OrbitMotion_BeginDistanceWave(void *, const void *, const void *, u32, u32);
static inline void Difference(struct SceneVec *out, struct SceneVec *a, struct SceneVec *b)
{
    out->x = (u16)a->x - (u16)b->x;
    out->y = (u16)a->y - (u16)b->y;
    out->z = (u16)a->z - (u16)b->z;
}
static inline u8 MotionEnded(struct ActorStatusPart *p)
{
    u8 result = 0;
    if (p->motion[0x29] == 2)
        result = 1;
    return result;
}
void ActorStatus_UpdateFlankingStrike(u8 *boss, struct ActorStatusPart *p)
{
    struct SceneVec target;
    u8 *enter;
    u32 mask = 8;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        p->state = 0;
        p->timer = 0;
        p->enter = 1;
    }
    switch (p->state) {
        case 0: {
            u32 entered;
            {
                u8 *address = &p->enter;
                entered = *address;
                enter = address;
            }
            if (entered) {
                u32 zero = 0;
                *enter = zero;
                Difference(&target, Player_GetEffectivePosition(zero), p->anchor);
                if (p->variant == 1) {
                    target.x += 512;
                    target.z -= 512;
                } else {
                    target.x -= 512;
                    target.z += 512;
                }
                OrbitMotion_BeginDistanceWave(p->motion, &p->offset, &target,
                                              ((p->variant * 128) + 96) & 255, 4);
            }
            MotionTrack_Update(p->motion);
            if (MotionEnded(p)) {
                u32 next = 1;
                p->state = next;
                p->timer = 0;
                *enter = next;
            }
            break;
        }

        case 1: {
            struct DisplaySequenceState *seq;
            u32 entered;
            {
                u8 *address = &p->enter;
                entered = *address;
                enter = address;
            }
            seq = &p->sequence;
            if (entered) {
                u32 zero = 0;
                *enter = zero;
                Anim_SetSequence(seq, *((void **)(boss + 0x4d0)), 8, (u8)p->variant, zero);
            }
            if (PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)p, seq)) {
                {
                    u32 next = 2;
                    p->state = next;
                }
                p->timer = 0;
                *enter = 1;
            }
            break;
        }

        case 2:
            p->finished = 1;
            break;
    }
}
