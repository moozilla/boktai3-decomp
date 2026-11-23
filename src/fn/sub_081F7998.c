#include "global.h"
#include "ratatosk.h"
void Sound_StopSong(u32);
void Ratatosk_BeginInterruptState(struct Ratatosk *p)
{
    u32 v, z;
    p->savedPhase = p->actionCounter;
    p->savedTime = p->callbackFrame;
    p->savedState = p->actionPhase;
    p->savedSequence = p->sequence;
    v = 8;
    z = 0;
    p->actionPhase = v;
    p->actionTimer = z;
    p->actionEnter = 1;
    Sound_StopSong(0x570);
}
