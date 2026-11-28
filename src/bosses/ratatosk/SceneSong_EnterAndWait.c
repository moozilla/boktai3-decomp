#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

void Sound_PlaySong(u32);

void SceneSong_EnterAndWait(struct Ratatosk *p)
{
    struct Ratatosk *actor = p;
    if (Ratatosk_ConsumeActionEntry(p)) {
        Ratatosk_SetAnimation(actor, 0x13);
        Sound_PlaySong(0x559);
    }
    if (actor->actionFinished)
        p->sequenceDone = 1;
}
