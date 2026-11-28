#include "monsters/ivy/phase_object.h"
#include "engine/engine_globals.h"
#include "engine/ram.h"
#include "engine/math/movement_curve.h"

extern s16 gUnk_086149C4[];
s32 Div(s32, s32);
void PhaseObject_BeginEntryMotion(struct PhaseObject *);

// confidence=low: steps the four entry weights and, on a timer, re-aims the pulse at the target.
void PhaseObject_UpdateEntryScatter(struct PhaseObject *p)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        p->offsetX = -p->offsetX;
        p->offsetZ = -p->offsetZ;
        p->entries[i].presentation.x += p->offsetX;
        p->entries[i].presentation.z -= p->offsetZ;
        p->entries[i].presentation.opacity -= 2;
        if (p->entries[i].presentation.opacity <= 1)
            p->entries[i].presentation.opacity = 1;
    }
    if (!(p->frame & 1)) {
        p->scatterStepX++;
        p->scatterStepZ++;
        p->offsetX = p->scatterStepX;
        p->offsetZ = p->scatterStepZ;
        if ((s16)p->scatterStepX > 15) {
            u32 angle;

            p->offsetX = p->entries[0].presentation.x - p->position.x;
            p->offsetZ = p->entries[0].presentation.z - p->position.z;
            angle = MovementCurve_RandomByte();
            p->position.x =
                p->target->position.x + Div(gUnk_086149C4[(angle + 64) & 255] * 200, 4096);
            p->position.y = p->target->position.y + 350;
            p->position.z = p->target->position.z + Div(gUnk_086149C4[angle & 255] * 200, 4096);
            for (i = 0; i < 4; i++) {
                p->entries[i].flags |= 1;
                p->entries[i].presentation.x = p->saved[i].x = p->position.x + p->offsetX;
                p->entries[i].presentation.z = p->saved[i].z = p->position.z + p->offsetZ;
                p->goal.x = p->position.x;
                p->goal.y = p->position.y;
                p->goal.z = p->position.z;
                p->offsetX = -p->offsetX;
                p->offsetZ = -p->offsetZ;
                p->entries[i].phase = 0x80;
            }
            p->rotation = 0x80;
            p->scatterDelay = 30;
            p->scatterTransition = PhaseObject_BeginEntryMotion;
        }
    }
}
