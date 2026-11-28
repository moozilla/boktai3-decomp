#include "global.h"
#include "player/player.h"

void Player_InitializeActionState(struct Player *, const struct PlayerActionConfig *);

void Player_InitConfiguredAction(struct Player *p)
{
    Player_InitializeActionState(
        p, (const struct PlayerActionConfig *)((const u8 *)p->actionConfig + 0x1C));
}
