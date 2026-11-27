#include "global.h"
#include "npc/kuro/kuro_command_actor_layout.h"
void Trigger_ConsumePending(void *, s32);
void Kuro_UpdateMode25(u32 a, struct KuroCommandActor *p)
{
    if (p->b8 != 0) {
        p->b8 = 0;
        p->b7 = 0;
        p->b5 = 0x16;
    }
    if (p->b7 != 0) {
        u32 o = 1;
        p->b4 = o;
        p->c14 = 0;
        p->b8 = o;
        Trigger_ConsumePending(&p->d, 1);
    }
}
