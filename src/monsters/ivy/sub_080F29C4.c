#include "monsters/ivy/phase_object.h"
s32 Div(s32, s32);
void PhaseObject_TickEntryReset(struct PhaseObject *);
void Ivy_UpdatePositionTransition(struct PhaseObject *p)
{
    u32 age;
    p->position.x = p->start.x + Div(p->delta.x * p->turnFrames, 16);
    p->position.y = p->start.y + Div(p->delta.y * p->turnFrames, 16);
    p->position.z = p->start.z + Div(p->delta.z * p->turnFrames, 16);
    age = p->turnFrames++;
    if ((u16)age > 16) {
        s32 i;
        void (*callback)(struct PhaseObject *);
        p->turnFrames = 0;
        p->position.x = p->goal.x;
        p->position.y = p->goal.y;
        p->position.z = p->goal.z;
        callback = PhaseObject_TickEntryReset;
        for (i = 0; i < 4; i++)
            p->entries[i].flags |= 1;
        p->transition = callback;
    }
}
