#define Ratatosk_UpdatePointEffectAnimation sub_081F4A6C
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
    u8 padB4[0x50];
    void *callback;
    u8 pad108[0xc];
    s8 savedAngle;
    u8 pad115[0x67];
    u32 effectCounter;
    u8 pad180[0x1ac];
    u8 animation[0x20];
    u8 animationData[0xb06];
    u16 motionE52, motionE54, padE56, motionE58;
    u8 padE5A[0x2a];
    s16 effectProgress;
    u8 padE86[0x7e];
    s32 completionFrame;
};
s32 Mod(s32, s32);
s32 Ratatosk_GetSelectedPointIndex(void);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
void Sound_StopSong(u32);
s32 Callback_InvokeActor(void *, void *);
void sub_081FEA80(s32);
void sub_081FEADC(s32);
void sub_081FEB84(s32);
void sub_081F4A6C(struct RatatoskActor *p, s32 frame)
{
    if (p->changed) {
        p->changed = 0;
        SceneActors_StartUnmirroredAnimation(p, p->animationData, p->animation, 16, 1, 4);
        p->effectCounter = 0;
        sub_081FEA80(Ratatosk_GetSelectedPointIndex());
        Sound_PlaySong(0x570);
    }
    if (Mod(frame, 300) == 180)
        sub_081FEADC(Ratatosk_GetSelectedPointIndex());
    {
        u32 mask = 0x100000;
        p->flags = mask | p->flags;
    }
    if (frame <= 179)
        sub_081FEB84(Ratatosk_GetSelectedPointIndex());
    if (p->effectProgress > 240) {
        p->effectCounter = 0;
        p->finished = 1;
        Sound_StopSong(0x570);
    }
    Callback_InvokeActor(p, p->callback);
}
