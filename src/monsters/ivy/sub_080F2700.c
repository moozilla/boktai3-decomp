#include "monsters/ivy/phase_object.h"
void ColorPulse_AlignTargetHeading(struct PhaseObject *);
s32 Math_Atan2(s32, s32);
void Sound_PlaySong(u32);
void ColorPulse_PrepareTargetDash(struct PhaseObject *p)
{
    p->rotationTimer += p->turnSpeed;
    if (p->rotationTimer > 255) {
        p->rotationTimer = 0;
        Sound_PlaySong(361);
    }
    p->rotation += p->turnSpeed;
    if (!(p->frame & 7)) {
        if (++p->turnSpeed > 7)
            p->turnSpeed = 7;
    }
    if (++p->turnFrames > 150) {
        p->targetAngle = 32 - Math_Atan2(p->position.x - p->target->position.x,
                                         p->position.z - p->target->position.z);
        p->start.x = p->position.x;
        p->start.y = p->position.y;
        p->start.z = p->position.z;
        p->goal.x = p->target->position.x;
        p->goal.y = p->target->position.y + 350;
        p->goal.z = p->target->position.z;
        p->delta.x = p->goal.x - p->position.x;
        p->delta.y = p->goal.y - p->position.y;
        p->delta.z = p->goal.z - p->position.z;
        p->turnFrames = 0;
        p->transition = ColorPulse_AlignTargetHeading;
    }
}
