#define Hresvelgr_UpdateTimedPose sub_081A1BE0
#include "global.h"
struct MotionPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct Hresvelgr {
    u8 unknown0[0x5c];
    struct MotionPosition pose;
    u8 unknown64[0x38];
    u32 flags;
    u8 unknownA0[0xf];
    u8 finished;
    u8 unknownB0;
    u8 pending;
    u8 unknownB2[0xc2];
    u8 collider174[0x20];
    u8 collider194[0x20];
    u8 unknown1B4[0x13c];
    u8 action[8];
    u8 unknown2F8[0x24];
    s16 motion[4];
    u8 unknown324[0x80];
    struct MotionPosition timedPose;
};
void ActionState_AdvanceState(void *);
void ActionState_Reset(void *);
void ActionState_SetState(void *, u32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
static inline void SetPose(struct MotionPosition *p, s16 x, s16 y, s16 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}
void sub_081A1BE0(struct Hresvelgr *actor)
{
    struct Hresvelgr *saved = actor;
    u8 *action = actor->action;
    if (actor->pending) {
        actor->pending = 0;
        SceneActors_StartUnmirroredAnimation(actor, actor->collider194, actor->collider174, 20, 1,
                                             0);
        ActionState_Reset(action);
        ActionState_SetState(action, 0);
        Sound_PlaySong(0x4cf);
    }
    switch (*action) {
        case 0:
            if (!*(u32 *)(action + 4)) {
                SetPose(&actor->timedPose, 0, 64, 0);
                (*(u32 *)(action + 4))++;
            }
            actor->pose = actor->timedPose;
            if (--actor->timedPose.y <= 0)
                ActionState_AdvanceState(action);
            break;
        case 1:
            actor->finished = 1;
            break;
    }
    saved->motion[0] = 0;
    saved->motion[1] = 0;
    saved->motion[2] = 0;
    saved->motion[3] = 0;
    {
        u32 mask = 12;
        actor->flags |= mask;
    }
}
