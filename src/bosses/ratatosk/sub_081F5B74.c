#include "engine/shared/global.h"
#include "bosses/ratatosk/ratatosk.h"
void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);
void Sound_PlaySong(u32);
static inline void setFacing(struct Ratatosk *p, int angle)
{
    int shifted;
    u32 value;
    p->body.direction = angle;
    shifted = p->body.direction + 0x60;
    shifted >>= 6;
    value = 3;
    value &= shifted;
    p->facing = value;
    shifted = p->body.direction + 0x70;
    shifted >>= 5;
    value = 7;
    value &= shifted;
    p->facing8 = value;
}
static inline void setTimedMode(struct Ratatosk *p, s32 duration, u8 mode, u8 value)
{
    p->blendBank = duration;
    p->blendMode = mode;
    p->blendShift = value;
    if (p->blendMode == 2) {
        u32 value = 32;
        p->blendStep = value;
    } else if (p->blendMode == 1)
        p->blendStep = 0;
}
void SceneActor_BeginConditionalAnimation8(struct Ratatosk *p)
{
    if (p->actionChanged) {
        p->actionChanged = 0;
        if (p->locked)
            setFacing(p, p->savedAngle);
        SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 8, 1, 3, 4);
        if (p->paletteToggle) {
            SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 8, 1, 3, 4);
            setTimedMode(p, 500, 4, 5);
        } else {
            SceneActors_StartDirectionalAnimation(p, p->animationData, p->animation, 8, 0, 3, 4);
        }
        p->scaleX = 0;
        p->scaleZ = 0;
        p->scaleMode = 0;
    }
}
