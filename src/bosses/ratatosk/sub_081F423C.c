#include "engine/shared/global.h"
#include "bosses/ratatosk/ratatosk.h"

void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
s32 Callback_InvokeActor(void *, void *);
void PaletteEffect_RefreshPalette(void *);
void Player_ClearFlag740(void);
static inline void palette(struct Ratatosk *p, u32 duration, u8 rate, u8 mode, u32 zero)
{
    p->blendBank = duration;
    p->blendMode = mode;
    p->blendShift = rate;
    if (p->blendMode == 2) {
        u8 value = 32;
        p->blendStep = value;
    } else if (p->blendMode == 1)
        p->blendStep = zero;
}
void ActorMotion_UpdateTimedPaletteAnimation(struct Ratatosk *p, s32 frame)
{
    struct Ratatosk *q = p;
    Ratatosk_SetFacing(p, p->requestedAngle);
    if (p->actionChanged) {
        p->actionChanged = 0;
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 8, 1, 3, 4);
        palette(p, 500, 5, 4, 0);
        q->scaleX = 0;
        q->scaleZ = 0;
        q->scaleMode = 0;
        if (frame <= 299)
            goto updateFlags;
    } else if (frame <= 299)
        goto updateFlags;

    p->actionFinished = 1;
    PaletteEffect_RefreshPalette(q);
updateFlags:
    if (p->actionPhase == 4) {
        Player_ClearFlag740();
        Ratatosk_SetFlags(p, 8);
        Ratatosk_SetFlags(p, 16);
    }
    {
        Ratatosk_SetFlags(p, 0x100000);
        if (p->actionPhase != 4)
            p->flags |= 64;
    }
    Ratatosk_SetFlags(p, 0x1000000);
    Callback_InvokeActor(p, p->actionTryUpdate);
}
