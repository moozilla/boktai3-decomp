#include "global.h"
#include "player/player.h"

void sub_08239D38(struct Player *);

void LinkPlayer_FlushPendingEffects(struct Player *p)
{
    s32 i;
    s32 count = p->movementModifierActive;

    for (i = 0; i < count; i++)
        sub_08239D38(p);
}
