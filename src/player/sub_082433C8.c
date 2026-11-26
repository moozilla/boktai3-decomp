#include "engine/shared/global.h"
#include "engine/shared/player.h"
#include "engine/shared/contact_actor.h"

void Player_ApplyContact(struct Player *, struct ContactCollider *, struct ContactCollider *);

void Player_ApplyGatedContact(struct ContactCollider *source, struct ContactCollider *target,
                              struct Player *player)
{
    if (player->kind == 1) {
        if (ContactCollider_TestFlags(source, 0x200)) {
            if (!(target->responseFlags & 1))
                return;
            target->responseFlags &= ~1;
        }
        if (ContactCollider_TestFlags(source, 0x400)) {
            if (target->responseFlags & 2)
                return;
            target->responseFlags |= 2;
        }
        Player_ApplyContact(player, source, target);
    }
}
