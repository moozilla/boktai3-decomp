#include "actors/paired_scene_actor/paired_scene_actor.h"
#include "engine/shared/global.h"
// confidence=low: part approaches the player and chooses contact or timeout animation
#include "effects/shared_display_animation_wrapper/display_animation.h"
#include "engine/shared/scene_vec.h"
#include "player/player_cue/player_cue_sequence.h"
void MotionTrack_Update(void *);
struct SceneVec *Player_GetEffectivePosition(u32);
void Sound_PlaySong(u32);
static inline void SetAnimation(struct PairedSceneHalf *p, void *data, u32 id, u32 options)
{
    struct DisplaySequenceState *seq = &p->graphics.sequence;
    Anim_SetSequence(seq, data, id, (u8)p->mode, options);
    PlayerCue_StepSequence(&p->sprite, seq);
}
static inline void SetVector(struct SceneVec *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}
static inline void AddOffset(struct SceneVec *out, struct SceneVec *a, struct SceneVec *b)
{
    out->x = (u16)b->x + (u16)a->x;
    out->y = (u16)b->y + (u16)a->y;
    out->z = (u16)b->z + (u16)a->z;
}
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
static inline u32 PlayerNear(struct PairedSceneHalf *p)
{
    struct SceneVec *player = Player_GetEffectivePosition(0);
    s32 dx = player->x - p->sprite.position.x;
    s32 dz = player->z - p->sprite.position.z;
    u32 result;
    s32 distance = dx * dx + dz * dz;
    result = 0;
    if (distance <= 0x63ff)
        result = 1;
    return result;
}

static inline void RequestBehavior(struct PairedSceneHalf *p, u32 action)
{
    u32 mask = 0x100;
    if (!(p->actionFlags & mask)) {
        if ((u8)(p->requestedState - 25) > 1 || p->finished) {
            p->requestedState = action;
            p->state = action;
            p->entered = 1;
            p->finished = 0;
            p->stateTimer = 0;
            p->actionCounters[0] = 0;
            p->actionCounters[1] = 0;
        }
    }
}
static inline void SetState(struct PairedSceneHalf *p, u32 state)
{
    p->actionCounters[0] = state;
    p->actionCounters[1] = 0;
}
void ActorStatus_UpdateGuardedApproach(struct VanargandAttackPair *boss,
                                       struct PairedSceneHalf *part)
{
    struct VanargandAttackPair *owner = boss;
    struct PairedSceneHalf *p = part;
    struct SceneVec target;
    struct SceneVec delta;
    u8 *enter;
    u32 mask = 0x28;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        SetAnimation(p, (void *)owner->effectResource, 11, 0);
        PairedSceneHalf_BeginActionPhase(p, 0);
        Sound_PlaySong(0x58a);
    }
    {
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
                    SetVector(&target, -1024, zero, -1024);
                    {
                        struct SceneVec *origin = (void *)&p->offset;
                        AddOffset(&target, origin, &target);
                        MotionTrack_Initialize(&p->motion, origin, &target, 0, 4);
                    }
                }
                MotionTrack_Update(&p->motion);
                if (MotionEnded(p)) {
                    SetState(p, 1);
                    *enter = 1;
                }
                break;
            }

            case 1: {
                u32 entered;
                {
                    u8 *address = &p->actionEntered;
                    entered = *address;
                    enter = address;
                }
                if (entered) {
                    u32 zero = 0;
                    *enter = zero;
                    Difference(&delta, Player_GetEffectivePosition(zero),
                               (struct SceneVec *)p->origin);
                    MotionTrack_Initialize(&p->motion, &p->offset, &delta, 0, 4);
                }
                MotionTrack_Update(&p->motion);
                if (PlayerNear(p)) {
                    RequestBehavior(p, 18);
                } else if (MotionEnded(p)) {
                    SetState(p, 2);
                    *enter = 1;
                }
                break;
            }

            case 2: {
                u32 entered;
                {
                    u8 *address = &p->actionEntered;
                    entered = *address;
                    enter = address;
                }
                if (entered) {
                    u32 zero = 0;
                    *enter = zero;
                    SetAnimation(p, (void *)owner->effectResource, 12, zero);
                    Sound_PlaySong(0x58a);
                }
                if ((++p->actionCounters[1]) > 120) {
                    SetAnimation(p, (void *)owner->effectResource, 9, 0);
                    SetState(p, 3);
                    {
                        u8 *e = enter;
                        *e = 1;
                    }
                }
                break;
            }

            case 3:
                p->finished = 1;
                break;
        }
    }
}
