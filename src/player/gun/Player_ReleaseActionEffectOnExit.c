#include "global.h"
#include "player/player.h"

void ProjectileTask_RemoveEntry(s32);

void Player_ReleaseActionEffectOnExit(struct Player *p)
{
    if (p->statModifierContext.attachedEntry >= 0)
        ProjectileTask_RemoveEntry(p->statModifierContext.attachedEntry);
}
