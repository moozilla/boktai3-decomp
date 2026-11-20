#include "sword_wave_component.h"

struct TargetMotionCorrection {
    u32 x : 16;
    s32 y : 16;
    u32 z : 16;
    u32 pad : 16;
};
s32 Div(s32, s32);
void IvyChild_UpdateAfterBob(struct SwordWaveComponent *);
void TargetMotion_AdvanceFollowSegment(struct SwordWaveComponent *p)
{
    struct TargetMotionCorrection correction;
    correction.x = ((u16 *)p->parent)[0] - p->a;
    correction.y = ((u16 *)p->parent)[1] - p->b;
    correction.z = ((u16 *)p->parent)[2] - p->c;
    p->rotation += p->rotationStep;
    p->x = correction.x + p->startX + Div(p->dx * p->timer, 100);
    p->y = correction.y + p->startY + Div(p->dy * p->timer, 100);
    p->z = correction.z + p->startZ + Div(p->dz * p->timer, 100);
    p->timer++;
    if (p->timer > 100) {
        p->done = 1;
        p->rotation = 128;
        SwordWaveComponent_SetAction(p, 1, 1);
        p->update = IvyChild_UpdateAfterBob;
    }
}
