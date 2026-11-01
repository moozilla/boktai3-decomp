// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: provisional role inferred from state transitions and player-motion calls

// confidence=low: part approaches the player and chooses contact or timeout animation
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
    u8 unknown171[7];
    u32 countdown;
};
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void MotionTrack_Update(void *);
struct SceneVec *Player_GetEffectivePosition(u32);
void Sound_PlaySong(u32);
static inline void SetAnimation(struct ActorStatusPart *p, u8 *boss, u32 id, u32 options)
{
    void *data = *(void **)(boss + 0x4d0);
    struct DisplaySequenceState *seq;
    if (id == 9) {
        void *resource = data;
        register u32 zero asm("r2");
        zero = 0;
        data = resource;
        options = zero;
    }
    seq = &p->sequence;
    Anim_SetSequence(seq, data, id, (u8)p->variant, options);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)p, seq);
}
static inline void SetVector(struct SceneVec *p, s32 x, s32 y, s32 z)
{
    ((volatile s16 *)p)[0] = x;
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
static inline u8 MotionEnded(struct ActorStatusPart *p)
{
    u8 result = 0;
    if (p->motion[0x29] == 2)
        result = 1;
    return result;
}
static inline u32 PlayerNear(struct ActorStatusPart *p)
{
    struct SceneVec *player = Player_GetEffectivePosition(0);
    s32 dx = player->x - p->position.x;
    s32 dz = player->z - p->position.z;
    u32 result;
    s32 distance = dx * dx + dz * dz;
    result = 0;
    if (distance <= 0x63ff)
        result = 1;
    return result;
}

static inline void RequestBehavior(struct ActorStatusPart *p, u32 action)
{
    u32 mask = 0x100;
    if (!(p->flags & mask)) {
        if ((u8)(p->previousAction - 25) > 1 || p->finished) {
            p->previousAction = action;
            p->action = action;
            p->reset = 1;
            p->finished = 0;
            p->countdown = 0;
            p->state = 0;
            p->timer = 0;
        }
    }
}
void ActorStatus_UpdateGuardedApproach(u8 *boss, struct ActorStatusPart *part)
{
    u8 *owner = boss;
    struct ActorStatusPart *p = part;
    struct SceneVec target;
    struct SceneVec delta;
    u8 *enter;
    u32 mask = 0x28;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        SetAnimation(p, owner, 11, 0);
        {
            u32 zero = 0;
            p->state = zero;
            p->timer = zero;
        }
        p->enter = 1;
        Sound_PlaySong(0x58a);
    }
    {
        register u32 offset asm("r1") = 0x164;
        u32 *state;
        state = (u32 *)(((u8 *)p) + offset);
        switch (*state) {
            case 0: {
                u32 entered;
                register void *motion asm("r6");
                {
                    u8 *address = &p->enter;
                    entered = *address;
                    enter = address;
                }
                motion = p->motion;
                if (entered) {
                    u32 zero = 0;
                    *enter = zero;
                    SetVector(&target, -1024, zero, -1024);
                    {
                        struct SceneVec *origin = &p->offset;
                        AddOffset(&target, origin, &target);
                        MotionTrack_Initialize(motion, origin, &target, 0, 4);
                    }
                }
                MotionTrack_Update(motion);
                if (MotionEnded(p)) {
                    u32 next = 1;
                    p->state = next;
                    p->timer = 0;
                    *enter = next;
                }
                break;
            }

            case 1: {
                u32 entered;
                register void *motion asm("r6");
                {
                    u8 *address = &p->enter;
                    entered = *address;
                    enter = address;
                }
                motion = p->motion;
                if (entered) {
                    u32 zero = 0;
                    *enter = zero;
                    Difference(&delta, Player_GetEffectivePosition(zero), p->anchor);
                    MotionTrack_Initialize(motion, &p->offset, &delta, 0, 4);
                }
                MotionTrack_Update(motion);
                if (PlayerNear(p)) {
                    RequestBehavior(p, 18);
                } else if (MotionEnded(p)) {
                    {
                        u32 next = 2;
                        p->state = next;
                    }
                    p->timer = 0;
                    *enter = 1;
                }
                break;
            }

            case 2: {
                u32 entered;
                {
                    register u8 *address = &p->enter;
                    entered = *address;
                    enter = address;
                }
                if (entered) {
                    u32 zero = 0;
                    *enter = zero;
                    SetAnimation(p, owner, 12, zero);
                    Sound_PlaySong(0x58a);
                }
                if ((++p->timer) > 120) {
                    SetAnimation(p, owner, 9, 0);
                    {
                        register u32 next = 3;
                        p->state = next;
                    }
                    p->timer = 0;
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
