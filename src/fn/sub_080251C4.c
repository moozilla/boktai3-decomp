#include "global.h"
#include "render.h"
typedef struct ActorAnimation {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[5];
    u8 finished, enter;
    u8 changed;
    u8 padB2;
    u8 locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x37];
    u8 (*tryUpdate)(void *);
    u8 (*reset)(void *);
    u8 pad10C[8];
    s8 savedAngle;
    u8 pad115[0x22e];
    u8 variant;
    s16 scale, minScale, maxScale, scaleStep;
    u8 pad34C[0x60];
    u32 mode;
} ActorAnimation;

static inline void setFacing(ActorAnimation *p, int angle)
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
void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);
void PolarInterpolation_SetCurrentFromPoint(void *, void *);
void sub_082222BC(void *, void *, int);
int Interp_Step(void *, void *);
void Sound_PlaySongChecked(int);
int SceneActorGraphics_GetResourceId(void *);
void TemporaryAnimationPool_CreateEntry(void *, int, int, int, void *);
void SceneActorAnimation_UpdateChargeApproach(ActorAnimation *p, int tick)
{
    ActorAnimation *p2 = p;
    u8 *position = (u8 *)p + 0x48;
    setFacing(p, p->requestedAngle);
    if (p->changed) {
        int sequence;
        p->changed = 0;
        {
            int test = p->mode & 1;
            sequence = 38;
            if (test)
                sequence = 0;
        }
        {
            int offset = 0x240;
            SceneActors_StartEightDirectionAnimation(p, (u8 *)p + offset, (u8 *)p + 0x220, sequence,
                                                     1, 1, 4);
        }
        PolarInterpolation_SetCurrentFromPoint((u8 *)p + 0x30c, (u8 *)p + 0x54);
        sub_082222BC((u8 *)p + 0x30c, (u8 *)p + 0x9c4, *(u16 *)((u8 *)p + 0x354));
        if (*(u16 *)((u8 *)p + 0x354) <= 64)
            Sound_PlaySongChecked(840);
        else
            Sound_PlaySongChecked(841);
    } else {
        int sequence;
        {
            int test = p->mode & 1;
            sequence = 38;
            if (test)
                sequence = 0;
        }
        {
            int offset = 0x240;
            SceneActors_StartEightDirectionAnimation(p, (u8 *)p + offset, (u8 *)p + 0x220, sequence,
                                                     1, 1, 1);
        }
    }
    if (!p->tryUpdate(p)) {
        u32 mask = 64;
        p->flags |= mask;
        if (!p->reset(p)) {
            if (!(tick & 3)) {
                int id = SceneActorGraphics_GetResourceId(p2);
                {
                    u8 *data = gPaletteBankData + id * 32;
                    TemporaryAnimationPool_CreateEntry((u8 *)p2 + 0x240, 8, 0x200, 0x9000, data);
                }
            }
            if (Interp_Step((u8 *)p2 + 0x30c, position + 12))
                p->finished = 1;
        }
    }
}
