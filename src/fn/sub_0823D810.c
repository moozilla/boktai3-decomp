#include "global.h"
#include "player.h"
#include "contact_collider.h"

u32 Collision_CalculateElementalResponse(struct ContactCollider *, struct ContactCollider *);
s32 sub_08249244(void *, struct ContactCollider *, struct ContactCollider *, void *);
s32 PlayerLink_IsActionModeAllowed(void *);

void Player_ApplyContactCosts(struct ContactCollider *source, struct ContactCollider *target)
{
    struct Player *player;
    s32 i;
    if (ContactCollider_TestContactFlags(target, 0x1F) != 0) {
        player = source->owner;
        Collision_CalculateElementalResponse(source, target);
        i = 0;
        do {
            void *cb = player->contactCostCallbacks[i];
            if (cb)
                target->cost += sub_08249244(player, source, target, cb);
            i++;
        } while (i <= 2);
        if (player->actionMode == 1 && player->statModifierContext.savedActionIndex == 1 &&
            PlayerLink_IsActionModeAllowed(player) != 0) {
            if (!(target->costFlags & 8)) {
                target->cost >>= 2;
                if (target->flags & 0x100)
                    target->costFlags |= 8;
            }
        }
    }
}
