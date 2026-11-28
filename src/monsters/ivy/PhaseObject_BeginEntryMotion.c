#include "monsters/ivy/phase_object.h"

void Ivy_InitBobbing(struct PhaseObject *);
void PhaseObject_ConvergeEntries(struct PhaseObject *);
void Sound_PlaySong(u32);

void PhaseObject_BeginEntryMotion(struct PhaseObject *p)
{
    if (--p->scatterDelay == 0) {
        s32 i;

        for (i = 0; i < 4; i++) {
            p->entries[i].presentation.y = p->goal.y;
            p->entries[i].flags &= ~1;
        }
        Ivy_InitBobbing(p);
        p->scatterDelay = 15;
        p->bobbingTimer = 20;
        p->scatterTransition = PhaseObject_ConvergeEntries;
        p->rotation = 0x80;
        Sound_PlaySong(0x16c);
    }
}
