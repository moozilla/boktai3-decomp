#include "global.h"

void sub_0802B7C8(u8 *p);
void GridAttack_UpdateActionController(u8 *p);
void GridAttack_RunStateIfUnpaused(u8 *p);
void sub_0802BAFC(u8 *p);
void GridAttackActor_RequestLinkedActorState(u8 *p);
void GridAttackActor_NotifyLinkedActorLatePhase(u8 *p);

int GridAttack_Update(u8 *p)
{
    u16 *t;
    sub_0802B7C8(p);
    GridAttack_UpdateActionController(p);
    GridAttack_RunStateIfUnpaused(p);
    sub_0802BAFC(p);
    if (*(u16 *)(p + 0x1aa) <= 0xef)
        GridAttackActor_RequestLinkedActorState(p);
    else
        GridAttackActor_NotifyLinkedActorLatePhase(p);
    t = (u16 *)(p + 0x1aa);
    (*t)++;
    if (*t > 0x12b)
        *t = 0;
    return 0;
}
