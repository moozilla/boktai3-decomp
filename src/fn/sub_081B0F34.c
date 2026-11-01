#include "global.h"
// confidence=low: provisional role inferred from state transitions and player-motion calls

// confidence=low: carries a player position request along a motion track
#include "display_animation.h"
#include "scene_vec.h"
#include "player.h"
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
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void MotionTrack_Update(void *);
void Player_BeginPositionRequest(void *, const void *, u32, u32, u32, u32, u32, u32, u32);
static inline u32 StepPlayback(struct ActorStatusPart *owner, struct DisplaySequenceState *sequence,
                               struct ActorStatusPart *base, u32 one)
{
    return PlayerCue_StepSequenceEnded(owner, &base->sequence);
}

static inline void SetAnimation(struct ActorStatusPart *p, u8 *boss, u32 id, u32 options)
{
    void *data = *(void **)(boss + 0x4d0);
    register struct DisplaySequenceState *seq = &p->sequence;
    Anim_SetSequence(seq, data, id, (u8)p->variant, options);
    StepPlayback(p, seq, p, 1);
}
static inline void SetVector(struct SceneVec *p, s32 x, s32 y, s32 z)
{
    ((volatile s16 *)p)[0] = x;
    p->y = y;
    p->z = z;
}
static inline u32 TestFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline u8 MotionEnded(struct ActorStatusPart *p)
{
    u8 result = 0;
    if (p->motion[0x29] == 2)
        result = 1;
    return result;
}
void ActorStatus_UpdatePlayerCapture(u8 *boss, struct ActorStatusPart *p)
{
    struct SceneVec target;
    struct SceneVec playerTarget;
    u32 mask = 12;
    typeof(mask) t1;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        SetAnimation(p, boss, 12, 0);
        {
            register u32 zero = 0;
            p->state = zero;
            p->timer = zero;
        }
        p->enter = 1;
    }
    mask = (t1 = 16);
    p->flags |= t1;
    {
        register u32 currentState;
        register u32 offset = 0x164;
        register u32 *stateAddress;
        stateAddress = (u32 *)(((u8 *)p) + offset);
        currentState = *stateAddress;
        switch (currentState) {
            case 0: {
                u8 *enter;
                u8 *motion;
                register u32 entered;
                {
                    register u8 *address = &p->enter;
                    entered = *address;
                    enter = address;
                }
                if (entered) {
                    *enter = currentState;
                    SetVector(&target, 640, 528, 640);
                    MotionTrack_Initialize(p->motion, &p->offset, &target, 0, 4);
                }
                {
                    volatile s16 *out = (void *)(&playerTarget);
                    struct SceneVec *position = &p->position;
                    out[0] = position->x;
                    out[1] = position->y - 128;
                    out[2] = position->z;
                }
                if (!p->variant) {
                    u8 *player = (*(u8 **)gPlayers);
                    u32 one = 1;
                    if (TestFlags((u32 *)(player + 0x20), 0x40000))
                        goto complete;
                    Player_BeginPositionRequest(player, &playerTarget, 0, 0, one, currentState,
                                                currentState, currentState, one);
                    motion = p->motion;
                } else {
                    if (TestFlags((u32 *)((*(u8 **)gPlayers) + 0x20), 0x40000))
                        goto complete;
                    Player_BeginPositionRequest((*(u8 **)gPlayers), &playerTarget, 0, 0,
                                                currentState, currentState, currentState,
                                                currentState, 2);
                    motion = p->motion;
                }
                MotionTrack_Update(motion);
                if (MotionEnded(p)) {
                complete: {
                    register u32 next = 1;
                    p->state = next;
                    p->timer = 0;
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
