#include "actors/phase_object/phase_object.h"

void ColorPulse_BeginNearApproach(struct PhaseObject *);
void PhaseObject_TickEntryReset(struct PhaseObject *);
void Sound_PlaySong(u32);
void ColorPulse_AlignTargetHeading(struct PhaseObject *p)
{
    s32 i;
    p->rotationTimer += p->turnSpeed;
    if (p->rotationTimer > 255) {
        p->rotationTimer = 0;
        Sound_PlaySong(361);
    }
    for (i = 0; i < p->turnSpeed; i++) {
        p->rotation++;
        if (p->rotation == p->targetAngle) {
            p->rotation = p->targetAngle;
            p->turnFrames = 0;
            p->turnSpeed = 0;
            if (p->delta.x * p->delta.x + p->delta.z * p->delta.z < 1000000) {
                p->transition = ColorPulse_BeginNearApproach;
            } else {
                s32 j;
                for (j = 0; j < 4; j++)
                    p->entries[j].flags |= 1;
                p->transition = PhaseObject_TickEntryReset;
            }
            return;
        }
    }
}
