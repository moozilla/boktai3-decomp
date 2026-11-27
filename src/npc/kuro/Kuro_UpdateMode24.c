#include "global.h"
#include "npc/kuro/kuro_command_actor_layout.h"
void Trigger_ConsumePending(void *, s32);
void Kuro_UpdateMode24(u32 a, struct KuroCommandActor *p)
{
    if (p->b8 != 0) {
        p->b8 = 0;
        p->b7 = 0;
        p->b5 = 0x15;
        Trigger_ConsumePending(&p->d, 1);
    }
}
