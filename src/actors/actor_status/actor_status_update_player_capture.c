#include "actors/paired_scene_actor/paired_scene_actor.h"
#include "engine/shared/global.h"
#include "effects/shared_display_animation_wrapper/display_animation.h"
#include "engine/shared/scene_vec.h"
#include "player/player.h"
#include "player/player_cue/player_cue_sequence.h"
void MotionTrack_Update(void *);
void Player_BeginPositionRequest(void *, const void *, u32, u32, u32, u32, u32, u32, u32);
static inline void SetAnimation(struct PairedSceneHalf *p, struct VanargandAttackPair *boss, u32 id,
                                u32 options)
{
    void *data = (void *)boss->effectResource;
    register struct DisplaySequenceState *seq = &p->graphics.sequence;
    Anim_SetSequence(seq, data, id, (u8)p->mode, options);
    PlayerCue_StepSequenceEnded(&p->sprite, seq);
}
static inline void SetVector(struct SceneVec *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}
static inline u32 TestFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline u8 MotionEnded(struct PairedSceneHalf *p)
{
    u8 result = 0;
    if (p->motion.active == 2)
        result = 1;
    return result;
}
void ActorStatus_UpdatePlayerCapture(struct VanargandAttackPair *boss, struct PairedSceneHalf *p)
{
    struct SceneVec target;
    struct SceneVec playerTarget;
    u32 mask = 12;
    typeof(mask) t1;
    p->actionFlags |= mask;
    if (p->entered) {
        p->entered = 0;
        SetAnimation(p, boss, 12, 0);
        PairedSceneHalf_BeginActionPhase(p, 0);
    }
    mask = (t1 = 16);
    p->actionFlags |= t1;
    {
        register u32 currentState;
        currentState = p->actionCounters[0];
        switch (currentState) {
            case 0: {
                u8 *enter;
                struct MotionTrack *motion;
                register u32 entered;
                {
                    register u8 *address = &p->actionEntered;
                    entered = *address;
                    enter = address;
                }
                if (entered) {
                    *enter = currentState;
                    SetVector(&target, 640, 528, 640);
                    MotionTrack_Initialize(&p->motion, &p->offset, &target, 0, 4);
                }
                {
                    struct SceneVec *out = &playerTarget;
                    struct SceneVec *position = (void *)&p->sprite.position;
                    out->x = position->x;
                    out->y = position->y - 128;
                    out->z = position->z;
                }
                if (!p->mode) {
                    u8 *player = (*(u8 **)gPlayers);
                    u32 one = 1;
                    if (TestFlags((u32 *)(player + 0x20), 0x40000))
                        goto complete;
                    Player_BeginPositionRequest(player, &playerTarget, 0, 0, one, currentState,
                                                currentState, currentState, one);
                    motion = &p->motion;
                } else {
                    if (TestFlags((u32 *)((*(u8 **)gPlayers) + 0x20), 0x40000))
                        goto complete;
                    Player_BeginPositionRequest((*(u8 **)gPlayers), &playerTarget, 0, 0,
                                                currentState, currentState, currentState,
                                                currentState, 2);
                    motion = &p->motion;
                }
                MotionTrack_Update(motion);
                if (MotionEnded(p)) {
                complete: {
                    register u32 next = 1;
                    p->actionCounters[0] = next;
                    p->actionCounters[1] = 0;
                    *enter = next;
                }
                }
                break;
            }

            case 1:
                p->finished = currentState;
                break;
        }
    }
}
