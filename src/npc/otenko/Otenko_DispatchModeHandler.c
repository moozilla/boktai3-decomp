#include "npc/otenko/otenko_actor.h"

extern u32 gUnk_086124D4[];

void Otenko_DispatchModeHandler(void *context, struct OtenkoActor *npc)
{
    sub_08249240(context, npc, gUnk_086124D4[npc->state]);
}
