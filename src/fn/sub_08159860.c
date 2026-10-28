#include "global.h"
#include "player.h"
#include "contact_actor.h"
void PlayerStats_AddHundredths(struct Player *, s32);
void PlayerStats_AddContactHundredths(struct ContactCollider *source, struct ContactCollider *other,
                                      s32 amount)
{
    struct Player *player = gPlayers[0];
    s32 gain, product;
    if (player) {
        gain = 0;
        if (source->contactFlags == 1 && !(source->reactionFlags & 0x40)) {
            if (source->statRewardFlags & 0x10) {
                if (source->statRewardFlags & 0x20)
                    gain = 10;
                else if (source->statRewardFlags & 0x40)
                    gain = 20;
                else if (source->statRewardFlags & 0x80)
                    gain = 50;
            } else {
                if (source->statRewardFlags & 1)
                    gain = 10;
                else if (source->statRewardFlags & 2)
                    gain = 40;
                else if (source->statRewardFlags & 4)
                    gain = 20;
            }
        }
        product = player->statScale * gain;
        if (product >= 0)
            gain = product >> 6;
        else
            gain = -((-product) >> 6);
        PlayerStats_AddHundredths(player, gain);
        source->cost += amount;
    }
}
