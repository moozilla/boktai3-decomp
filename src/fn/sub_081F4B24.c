#define Ratatosk_UpdateShrinkingAnimation sub_081F4B24
#include "global.h"

/* Ratatosk field and state interpretations are provisional (low confidence). */
struct ActorAnimationChannel {
    u8 pad[8];
    u32 flags;
};
struct RatatoskActor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[4];
    u8 active, finished, padB0, changed, padB2, locked;
    u8 padB4[0x50];
    void *callback;
    u8 pad108[0xc];
    s8 savedAngle;
    u8 pad115[0x67];
    u32 effectCounter;
    u8 pad180[0x1ac];
    u8 animation[0x20];
    struct ActorAnimationChannel animationData;
    u8 pad358[0x16];
    u16 rotation;
    u8 pad370[0x10];
    u8 scaleX, scaleY;
    u8 pad382[0x8a];
    u8 shadow[0x40];
    u8 pad44C[0xa06];
    u16 motionE52, motionE54, padE56, motionE58;
    u8 padE5A[0x2a];
    s16 effectProgress;
    u8 padE86[0x52];
    u16 shrinkScale;
    u8 padEDA[0x2a];
    s32 completionFrame;
};
s32 Ratatosk_GetSelectedPointIndex(void);
void ScaledShadow_Disable(void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
void sub_081FEB84(s32);
void sub_081F4B24(struct RatatoskActor *p, s32 frame)
{
    if (p->changed) {
        p->changed = 0;
        ScaledShadow_Disable(p->shadow);
        {
            struct ActorAnimationChannel *animation = &p->animationData;
            SceneActors_StartUnmirroredAnimation(p, animation, p->animation, 25, 1, 4);
            animation->flags |= 2;
        }
        p->shrinkScale = 64;
        Sound_PlaySong(0x572);
    }
    p->shrinkScale -= 2;
    p->scaleX = p->shrinkScale;
    p->scaleY = p->shrinkScale;
    if (p->locked)
        p->rotation += 128;
    {
        u32 mask = 0x8000c;
        p->flags = mask | p->flags;
    }
    sub_081FEB84(Ratatosk_GetSelectedPointIndex());
    if (frame > 299 || p->shrinkScale <= 1) {
        p->shrinkScale = 1;
        {
            u32 mask = 16;
            p->flags = mask | p->flags;
        }
        p->finished = 1;
    }
    Callback_InvokeActor(p, p->callback);
}
