#include "phase_object.h"

s32 Div(s32, s32);

void ChildGroup_UpdateAlternatingCallbacks(struct PhaseObject *);
void PhaseObject_UpdateEntryScatter(struct PhaseObject *);
// confidence=low: interpolates the four entries toward the shared target before enabling them.
void PhaseObject_ConvergeEntries(struct PhaseObject *p)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        struct PhaseObject *entry;
        s32 x = Div((p->goal.x - p->saved[i].x) * p->scatterDelay, 15);
        entry = (void *)((u8 *)p + i * 0x2C);
        entry->entries[0].presentation.x = p->goal.x + x;
        entry->entries[0].presentation.z =
            p->goal.z + Div((p->goal.z - p->saved[i].z) * p->scatterDelay, 15);
        entry->entries[0].presentation.opacity += 3;
        if (entry->entries[0].presentation.opacity > 63) {
            entry->entries[0].presentation.opacity = 64;
        }
    }
    if (!(p->frame & 1) && --p->scatterDelay == 0) {
        u8 *enabled = &p->groupMode.active;
        u8 *mode = &p->groupMode.mode;
        u8 *flags8D = &p->groupMode.flags;
        s16 *targetX = &p->goal.x;
        s16 *targetZ = &p->goal.z;
        struct PhaseEntryPresentation *entry = &p->entries[0].presentation;
        u32 offset = 0;
        u32 opacityValue = 64;
        u32 *flags = &p->entries[0].flags;
        u32 bit = 1;
        for (i = 3; i >= 0; i--) {
            entry->x = *targetX;
            entry->z = *targetZ;
            entry->opacity = opacityValue;
            *(u32 *)((u8 *)flags + offset) |= bit;
            entry = (void *)((u8 *)entry + sizeof(struct PhaseEntry));
            offset += 0x2C;
        }
        p->drawableFlags &= ~1;
        p->colliderA.flags &= 0xFFFB;
        p->colliderB.flags &= 0xFFFB;
        {
            u8 one = 1;
            u32 five = 5;
            *enabled = one;
            *mode = five;
            *flags8D = one | *flags8D;
        }
        p->update = ChildGroup_UpdateAlternatingCallbacks;
        p->scatterTransition = PhaseObject_UpdateEntryScatter;
    }
}
