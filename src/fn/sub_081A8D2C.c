#define Vanargand_UpdateMotionTrack sub_081A8D2C
#define MotionTrack_Update          sub_081B95E4
#define PairedSceneActor_IsFinished sub_081B64D4
#include "global.h"
struct MotionTrackVector {
    s16 x, y, z, padding;
};
struct Vanargand {
    u8 pad0[0x54];
    u8 motionData[0x9C - 0x54];
    u32 flags;
    u8 padA0[0xAF - 0xA0];
    u8 finished, entered, pendingAnimation;
    u8 padB2[0x18C - 0xB2];
    u8 animation[0x60];
    u8 drawable[0x534 - 0x1EC];
    u8 track[0x29];
    u8 trackState;
    u8 pad55E[0x56C - 0x55E];
    void *attachment;
    u8 pad570[0x5C2 - 0x570];
    u16 counter;
};
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void PairedSceneActor_ApplyToSelectedHalves(void *, s32, s32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Vanargand_SetPendingCue(void *, u16);
void sub_081B95E4(void *);
s32 sub_081B64D4(void *);
static inline u8 complete(struct Vanargand *p)
{
    u8 result = 0;
    if (p->trackState == 2)
        result = 1;
    return result;
}
static inline s16 *vector(s16 *p, s16 x, s16 y, s16 z)
{
    s16 *q = p;
    *q = x;
    p[1] = y;
    p[2] = z;
    return p;
}
static inline void initialize(void *track, void *data, s16 x, s16 y, s16 z, s16 *target, u32 mode)
{
    s16 origin[4];
    origin[0] = x;
    origin[1] = y;
    origin[2] = z;
    MotionTrack_Initialize(track, data, origin, target, mode);
}
void sub_081A8D2C(struct Vanargand *p)
{
    struct Vanargand *p2 = p;
    s16 target[4];
    u32 mask = 12;
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 1);
    if (p->pendingAnimation) {
        p->pendingAnimation = 0;
        SceneActors_StartUnmirroredAnimation(p, p->animation, p->drawable, 2, 1, 4);
        {
            s16 *q = target;
            vector(q, 8, 8, 8);
        }
        initialize(p->track, p->motionData, 0x280, 0x300, 0x280, target, 5);
        PairedSceneActor_ApplyToSelectedHalves(p->attachment, 3, 0x13);
    } else
        MotionTrack_Update(p->track);
    if (complete(p2)) {
        if (PairedSceneActor_IsFinished(p2->attachment)) {
            p2->counter = 0;
            p->finished = 1;
        }
    }
}
