#define Player_AccumulateAction sub_081412C0
#define Player_TriggerAccumulatorEvent Player_EmitPeriodicEffect
#include "global.h"
#include "player.h"
void Player_EmitPeriodicEffect(struct Player *);
static inline u8 Player_IsInModeSeven(struct Player *p)
{
    u32 active = p->active;
    u32 result = 0;
    if (active && p->sceneMode == 7) result = 1;
    return result;
}
void sub_081412C0(struct Player *p, s32 amount)
{
    if (p->actionAccumulatorBlocked) return;
    if (p && Player_IsInModeSeven(p)) return;
    p->actionAccumulator += amount + 2;
    if (p->actionAccumulator > 99) {
        Player_EmitPeriodicEffect(p);
        p->actionAccumulator -= 100;
    }
}
