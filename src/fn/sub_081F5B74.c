#define SceneActor_BeginConditionalAnimation8 sub_081F5B74
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
    u8 padB4[0x60];
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
static inline void setTimedMode(TimedSceneActor *p, s32 duration, u8 mode, u8 value)
{
    p->timer4B0 = duration;
    p->mode4AE = mode;
    p->value4AF = value;
    if (p->mode4AE == 2) {
        u32 value = 32;
        p->value4AC = value;
    } else if (p->mode4AE == 1)
        p->value4AC = 0;
}
void sub_081F5B74(TimedSceneActor *p)
{
    if (p->enter) {
        p->enter = 0;
        if (p->locked)
            setFacing(p, p->savedAngle);
        SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x34c, (u8 *)p + 0x32c, 8, 1, 3, 4);
        if (p->conditionEE4) {
            SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x34c, (u8 *)p + 0x32c, 8, 1, 3, 4);
            setTimedMode(p, 500, 4, 5);
        } else {
            SceneActors_StartDirectionalAnimation(p, (u8 *)p + 0x34c, (u8 *)p + 0x32c, 8, 0, 3, 4);
        }
        p->valueE52 = 0;
        p->valueE54 = 0;
        p->valueE58 = 0;
    }
}
