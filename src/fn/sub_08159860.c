#define PlayerStats_AddContactHundredths sub_08159860
#define PlayerStats_AddHundredths        sub_0815977C
#define gPlayerSlots                     gUnk_02000580
#include "global.h"
#include "player.h"
#include "contact_actor.h"
void sub_0815977C(struct Player *, s32);
void sub_08159860(struct ContactCollider *source, struct ContactCollider *other, s32 amount)
{
    struct Player *player = gPlayerSlots[0];
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
        sub_0815977C(player, gain);
        source->cost += amount;
    }
}
