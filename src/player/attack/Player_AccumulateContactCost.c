#include "global.h"
#include "player/player.h"
#include "scene/collision/contact_actor.h"
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
                u32 *counter = &gGameState->unk600;

                (*counter)++;
                if (gGameState->unk600 == gGameState->contactThreshold * 3 >> 2) {
                    Sound_PlaySong(0x302);
                    p->actionEighteenParameter = 0x16F;
                    p->actionEighteenDuration = 0x40;
                    p->contactCueDuration = 0x40;
                } else if (gGameState->unk600 == gGameState->contactThreshold >> 1) {
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
