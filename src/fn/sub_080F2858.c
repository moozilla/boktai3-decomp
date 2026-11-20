#include "phase_object.h"

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
    i = 0;
    if (i < p->turnSpeed) {
        u8 *rotation = &p->rotation;
        u32 zero = 0;
        u16 *timer = &p->turnFrames;
        s8 *speed = &p->turnSpeed;
        void (**state)(struct PhaseObject *) = &p->transition;
        u8 *angle = &p->targetAngle;
        s16 *dx = &p->delta.x;
        s16 *dz = &p->delta.z;
        struct PhaseEntry *parts;
        parts = p->entries;
        do {
            if (++*rotation == *angle) {
                s32 x, z;

                *rotation = *angle;
                *timer = zero;
                *speed = zero;
                x = *dx * *dx;
                z = *dz * *dz;

                if (x + z < 1000000) {
                    *state = ColorPulse_BeginNearApproach;
                } else {
                    s32 partIndex;
                    for (partIndex = 0; partIndex < 4; partIndex++)
                        parts[partIndex].flags |= 1;
                    *state = PhaseObject_TickEntryReset;
                }
                return;
            }
            i++;
        } while (i < *speed);
    }
}
