#define Callback_InvokeWithArgument Callback_InvokeActor
#include "global.h"
typedef struct TimedSceneActor {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[5];
    u8 finished, padB0, enter, padB2, locked;
    u8 padB4[0x50];
    void *update;
    u8 pad108[0xc];
    s8 savedAngle;
    u8 pad115[0x397];
    u8 value4AC, pad4AD, mode4AE, value4AF;
    s32 timer4B0;
    u8 pad4B4[0x99e];
    s16 valueE52, valueE54, padE56, valueE58;
    u8 padE5A[0x8a];
    s16 conditionEE4;
    u8 padEE6[0x1e];
    s32 duration;
} TimedSceneActor;
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySong(u32);
static inline void setFacing(TimedSceneActor *p, int angle)
{
    int shifted;
    u32 value;
    p->angle = angle;
    shifted = p->angle + 0x60;
    shifted >>= 6;
    value = 3;
    value &= shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    shifted >>= 5;
    value = 7;
    value &= shifted;
    p->facing8 = value;
}
s32 Callback_InvokeActor(void *, void *);
void SceneActor_UpdateAnimation0Timer(TimedSceneActor *p, s32 elapsed)
{
    TimedSceneActor *self = p;
    if (p->enter) {
        p->enter = 0;
        setFacing(p, p->savedAngle);
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x34c, (u8 *)p + 0x32c, 0, 1, 1, 4);
    } else {
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x34c, (u8 *)p + 0x32c, 0, 1, 1, 1);
    }
    self->valueE52 = 0;
    self->valueE54 = 0;
    self->valueE58 = 0;
    {
        u32 mask = 0x100000;
        p->flags |= mask;
    }
    if (!(u8)Callback_InvokeWithArgument(p, p->update)) {
        if (elapsed >= self->duration && !p->locked)
            p->finished = 1;
        {
            u32 mask = 0x10;
            p->flags |= mask;
        }
    }
}
