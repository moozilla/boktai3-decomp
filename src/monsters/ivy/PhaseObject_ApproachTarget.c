#define gPhaseDirectionSines gUnk_086149C4
#include "monsters/ivy/phase_object.h"

// Tentative family name: object with four phase entries and timed state callbacks.
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
void PhaseObject_EndApproachPause(struct PhaseObject *);
void PhaseObject_ApproachTarget(struct PhaseObject *p)
{
    s32 vx, vz, stepX, stepZ;
    p->goal.x = p->target->position.x;
    p->goal.y = p->target->position.y;
    p->goal.z = p->target->position.z;
    p->delta.x = p->goal.x - p->position.x;
    p->delta.y = p->goal.y - p->position.y;
    p->delta.z = p->goal.z - p->position.z;
    p->targetAngle = Math_Atan2(p->delta.x, p->delta.z);
    vx = gUnk_086149C4[(p->targetAngle + 0x40) & 0xff];
    vz = gUnk_086149C4[p->targetAngle];
    if (vx >= 0)
        stepX = vx >> 10;
    else
        stepX = -(-vx >> 10);
    p->offsetX = stepX;
    if (vz >= 0)
        stepZ = vz >> 10;
    else
        stepZ = -(-vz >> 10);
    p->offsetZ = stepZ;
    p->position.x += p->offsetX;
    p->position.z += p->offsetZ;
    if (!(p->frame & 3)) {
        if (++p->countdown == 0x50)
            p->secondaryTransition = PhaseObject_EndApproachPause;
    }
}
