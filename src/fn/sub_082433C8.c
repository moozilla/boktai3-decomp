#define Player_ApplyGatedContact sub_082433C8
#define Player_ApplyContact sub_08243090
#include "global.h"
#include "player.h"
#include "contact_actor.h"

void sub_08243090(struct Player *, struct ContactCollider *, struct ContactCollider *);

static inline u32 testFlags(struct ContactCollider *p, u32 mask)
{
    return p->flags & mask;
}

void sub_082433C8(struct ContactCollider *source, struct ContactCollider *target, struct Player *player)
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
        sub_08243090(player, source, target);
    }
}
