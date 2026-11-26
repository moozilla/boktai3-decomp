#include "engine/shared/global.h"
#include "player/player.h"
#include "engine/shared/contact_actor.h"
#include "player/player_slot_status.h"
#define Player_CheckContactReduction Player_IsActionBlocked
void Collision_CalculateDamageResponse(void *, void *);
s32 Player_IsActionBlocked(struct Player *);
s32 sub_08249244(void *, void *, void *, void *);
void Sound_PlaySong(u32);
void Player_AccumulateContactCost(struct ContactCollider *source, struct ContactCollider *contact)
{
    struct Player *p;
    s32 i;
    if (ContactCollider_TestContactFlags(contact, 0x10)) {
        p = source->owner;
        Collision_CalculateDamageResponse(source, contact);
        if (p->actionMode == 1) {
            if (p->statModifierContext.savedActionIndex == 1 && Player_IsActionBlocked(p) &&
                !(contact->costFlags & 8)) {
                contact->cost >>= 2;
                if (contact->flags & 0x100)
                    contact->costFlags |= 8;
            }
            if ((Player_ReadStatusBits(&contact->reactionFlags, 0xF80)) && !p->contactCounted) {
                s32 total, count, threshold;
                s32 *counter;
                {
                    s32 *count = (s32 *)&gGameState->unk600;
                    (*count)++;
                }
                counter = (s32 *)&gGameState->unk600;
                total = gGameState->contactThreshold;
                threshold = total * 3 >> 2;
                count = *counter;
                if (count == threshold) {
                    Sound_PlaySong(0x302);
                    p->actionEighteenParameter = 0x16F;
                    p->actionEighteenDuration = 0x40;
                    p->contactCueDuration = 0x40;
                } else if (count == (total >> 1)) {
                    Sound_PlaySong(0x302);
                    p->actionEighteenParameter = 0x16F;
                    p->actionEighteenDuration = 0x20;
                    p->contactCueDuration = 0x20;
                }
                p->contactCounted = 1;
            }
        }
        for (i = 0; i < 3; i++) {
            void *callback = p->contactCostCallbacks[i];
            if (callback)
                contact->cost += sub_08249244(p, source, contact, callback);
        }
    }
}
