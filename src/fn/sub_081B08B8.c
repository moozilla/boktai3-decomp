// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: provisional role inferred from state transitions and player-motion calls

#include "display_animation.h"
#include "paired_scene_actor.h"
#include "player_cue_sequence.h"
struct OrbitMotion;
s32 MotionTrack_Update(struct OrbitMotion *);
s32 Vanargand_GetComponentWithFlag10(u8 *);
u8 *Player_GetEffectivePosition(u32);
void Sound_PlaySong(u32);
static inline void SetAnimation(struct PairedSceneHalf *p, void *data, u32 id, u32 options)
{
    struct DisplaySequenceState *seq = &p->graphics.sequence;
    Anim_SetSequence(seq, data, id, (u8)p->mode, options);
    PlayerCue_StepSequence(&p->sprite, seq);
}
static inline void SetVector(struct MotionTrackVector *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}
static inline void AddOffset(struct MotionTrackVector *out, struct ContactPosition *a,
                             struct MotionTrackVector *b)
{
    out->x = (u16)b->x + (u16)a->x;
    out->y = (u16)b->y + (u16)a->y;
    out->z = (u16)b->z + (u16)a->z;
}
static inline void Difference(struct MotionTrackVector *out, struct ContactPosition *a,
                              struct ContactPosition *b)
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
static inline u32 PlayerNear(struct PairedSceneHalf *p)
{
    struct ContactPosition *player = (struct ContactPosition *)Player_GetEffectivePosition(0);
    register u32 scratch asm("r3");
    s32 x, dx;
    s32 dz;
    u32 result;
    s32 distance;
    /* The opaque scratch value is used only by allocation constraints. */
    asm("" : "=r"(scratch));
    x = player->x;
    asm("" : : "r"(scratch));
    dx = x - p->sprite.position.x;
    dz = player->z - p->sprite.position.z;
    distance = dx * dx + dz * dz;
    result = 0;
    if (distance <= 0x63ff)
        result = 1;
    return result;
}

// confidence=low: part approaches the player and chooses contact or timeout animation
void ActorStatus_UpdatePlayerApproach(struct VanargandAttackPair *boss,
                                      struct PairedSceneHalf *part)
{
    struct VanargandAttackPair *owner = boss;
    struct PairedSceneHalf *p = part;
    struct MotionTrackVector target, delta;
    u8 *enter;
    u32 mask = 0x28;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        SetAnimation(p, (void *)owner->effectResource, 11, 0);
        {
            u32 zero = 0;
            p->actionCounters[0] = zero;
            p->actionCounters[1] = zero;
        }
        p->actionEntered = 1;
    }
    switch (p->actionCounters[0]) {
        case 0: {
            u32 entered;
            entered = p->actionEntered;
            enter = &p->actionEntered;
            if (entered) {
                u32 zero = 0;
                *enter = zero;
                SetVector(&target, -256, zero, -256);
                {
                    struct ContactPosition *origin = &p->offset;
                    AddOffset(&target, origin, &target);
                    MotionTrack_Initialize(&p->motion, origin, &target, 0, 4);
                }
            }
            MotionTrack_Update((struct OrbitMotion *)&p->motion);
            if (MotionEnded(p)) {
                u32 next = 1;
                p->actionCounters[0] = next;
                p->actionCounters[1] = 0;
                *enter = next;
            }
            break;
        }
        case 1: {
            u32 entered;
            entered = p->actionEntered;
            enter = &p->actionEntered;
            if (entered) {
                u32 zero = 0;
                *enter = zero;
                Difference(&delta, (struct ContactPosition *)Player_GetEffectivePosition(zero),
                           p->origin);
                MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 3);
            }
            MotionTrack_Update((struct OrbitMotion *)&p->motion);
            if (Vanargand_GetComponentWithFlag10((u8 *)owner) >= 0) {
                goto completed;
            } else if (PlayerNear(p)) {
                {
                    u32 next = 2;
                    p->actionCounters[0] = next;
                }
                p->actionCounters[1] = 0;
                *enter = 1;
            } else if (MotionEnded(p)) {
                {
                    u32 next = 3;
                    p->actionCounters[0] = next;
                }
                p->actionCounters[1] = 0;
                *enter = 1;
            }
            break;
        }
        case 2:
            if (p->actionEntered) {
                p->actionEntered = 0;
                SetAnimation(p, (void *)owner->effectResource, 12, 0);
                Sound_PlaySong(0x58a);
            }
            mask = 0x10;
            {
                u32 *flags = &p->actionFlags;
                *flags |= mask;
            }
            goto finished;
        case 3: {
            u32 entered;
            entered = p->actionEntered;
            enter = &p->actionEntered;
            if (entered) {
                u32 zero = 0;
                *enter = zero;
                SetAnimation(p, (void *)owner->effectResource, 12, zero);
                Sound_PlaySong(0x58a);
            }
            {
                u32 *timer = &p->actionCounters[1];
                if (++*timer > 30) {
                    SetAnimation(p, (void *)owner->effectResource, 9, 0);
                completed: {
                    u32 next = 4;
                    p->actionCounters[0] = next;
                }
                    p->actionCounters[1] = 0;
                    *enter = 1;
                }
            }
            break;
        }
        case 4:
        finished:
            p->finished = 1;
            break;
    }
}
