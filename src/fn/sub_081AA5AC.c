#include "global.h"
struct Vanargand {
    u8 pad0[0x54];
    u8 motionData[0xAF - 0x54];
    u8 finished, entered, pendingAnimation;
    u8 padB2[0x104 - 0xB2];
    void *callback;
    u8 pad108[0x18C - 0x108];
    u8 animation[0x60];
    u8 drawable[0x534 - 0x1EC];
    u8 track[0x29];
    u8 trackState;
};
s32 Callback_InvokeActor(void *, void *);
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Vanargand_SetPendingCue(void *, u16);
void MotionTrack_Update(void *);
static inline s16 *vector(s16 *p, s16 x, s16 y, s16 z)
{
    s16 *q = p;
    *q = x;
    p[1] = y;
    p[2] = z;
    return p;
}
static inline u8 complete(struct Vanargand *p)
{
    u8 result = 0;
    if (p->trackState == 2)
        result = 1;
    return result;
}
void Vanargand_UpdateMotionReturn(struct Vanargand *p)
{
    void *animation = p->animation;
    u8 result;
    s16 origin[4];
    Vanargand_SetPendingCue(p, 1);
    result = Callback_InvokeActor(p, p->callback);
    if (result)
        return;
    if (p->pendingAnimation) {
        p->pendingAnimation = result;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 2, 1, 4);
        vector(origin, 0x300, 0x200, 0x300);
        MotionTrack_Initialize(p->track, p->motionData, origin, 0, 4);
    }
    MotionTrack_Update(p->track);
    if (complete(p))
        p->finished = 1;
}
