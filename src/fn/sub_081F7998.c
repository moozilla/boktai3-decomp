#include "global.h"
#include "ratatosk.h"
void Sound_StopSong(u32);
void Ratatosk_BeginInterruptState(struct Ratatosk *p)
{
    p->savedPhase = p->actionCounter;
    p->savedTime = p->callbackFrame;
    p->savedState = p->actionPhase;
    p->savedSequence = p->sequence;
    Ratatosk_EnterPhase(p, 8);
    Sound_StopSong(0x570);
}
