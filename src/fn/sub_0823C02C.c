#define Player_AccumulateActionCharge sub_0823C02C
#include "global.h"
#include "player.h"
void sub_0823B308(struct Player *);
void sub_0823C02C(struct Player *s, s32 amount)
{
    s->actionAccumulator += amount + 2;
    if (s->actionAccumulator > 99) {
        sub_0823B308(s);
        s->actionAccumulator -= 100;
    }
}
