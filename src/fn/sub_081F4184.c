#define Ratatosk_UpdateTimedAttack sub_081F4184
#include "global.h"

/* Ratatosk field and state interpretations are provisional (low confidence). */
struct RatatoskActor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[4];
    u8 active, finished, padB0, changed, padB2, locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x37];
    void *callback;
    u8 pad108[0xc];
    s8 savedAngle;
    u8 pad115[0x217];
    u8 animation[0x20];
    u8 animationData[0xb06];
    u16 motionE52, motionE54, padE56, motionE58;
    u8 padE5A[0xaa];
    s32 completionFrame;
};
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);
static inline void facing(struct RatatoskActor *p, int angle)
{
    u32 v;
    int shifted;
    p->angle = angle;
    shifted = (p->angle + 0x60) >> 6;
    v = 3;
    v &= shifted;
    p->facing = v;
    shifted = (p->angle + 0x70) >> 5;
    v = 7;
    v &= shifted;
    p->facing8 = v;
}
void Sound_PlaySong(u32);
void sub_081F4184(struct RatatoskActor *p, s32 frame)
{
    facing(p, p->requestedAngle);
    if (p->changed) {
        p->changed = 0;
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 9, 2, 3, 4);
        Sound_PlaySong(0x55c);
    }
    if (frame == 13)
        Sound_PlaySong(0x565);
    if ((u32)(frame - 13) <= 12 && p->active != 5) {
        u32 mask = 0x20000;
        p->flags = mask | p->flags;
    }
    if (frame > 37)
        p->finished = 1;
    Callback_InvokeActor(p, p->callback);
}
