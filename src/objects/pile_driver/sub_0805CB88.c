#include "objects/pile_driver/pile_driver_subject.h"
void PileDriverSubject_SetState(struct PileDriverSubject *owner, u32 state)
{
    struct PileDriverSubject *subject = (void *)owner;
    subject->state = state;
    subject->stateFrame = 0;
    subject->update = gUnk_08605AF8[subject->state];
    if (subject->state == 3)
        Sound_PlaySong(0xFB);
    else if (subject->state == 5)
        Sound_PlaySong(0xFC);
}
