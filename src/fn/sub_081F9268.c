#include "global.h"
#include "ratatosk.h"

void SceneActors_ConfigureEffect(u8 *, unsigned, int, int);

void SceneEffect_EnterAndRestart(struct Ratatosk *p)
{
    if (Ratatosk_ConsumeActionEntry(p))
        SceneActors_ConfigureEffect((u8 *)p, 5, p->actionTimer, 120);
    if (p->actionFinished)
        Ratatosk_StartAction(p, 0, 0, 1);
    p->actionTimer++;
}
