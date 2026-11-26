#include "global.h"
#include "player/player.h"
void Player_SpawnActionSpark(struct Player *);
void Player_AccumulateActionCharge(struct Player *s, s32 amount)
{
    s->actionAccumulator += amount + 2;
    if (s->actionAccumulator > 99) {
        Player_SpawnActionSpark(s);
        s->actionAccumulator -= 100;
    }
}
