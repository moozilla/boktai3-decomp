#include "global.h"
#include "player.h"
#include "contact_actor.h"

void Player_ApplyContact(struct Player *, struct ContactCollider *, struct ContactCollider *);

static inline u32 testFlags(struct ContactCollider *p, u32 mask)
{
    return p->flags & mask;
}

void Player_ApplyGatedContact(struct ContactCollider *source, struct ContactCollider *target,
                              struct Player *player)
{
    if (player->kind == 1) {
        if (testFlags(source, 0x200)) {
            if (!(target->responseFlags & 1))
                return;
            target->responseFlags &= ~1;
        }
        if (testFlags(source, 0x400)) {
            if (target->responseFlags & 2)
                return;
            target->responseFlags |= 2;
        }
        Player_ApplyContact(player, source, target);
    }
}
