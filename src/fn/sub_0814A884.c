#include "global.h"
#include "player.h"
#include "contact_actor.h"
#define Player_CheckContactReduction Player_IsActionBlocked
struct PlayerContactSource {
    u8 unknown0[0x50];
    struct Player *owner;
};

void Collision_CalculateDamageResponse(void *, void *);
s32 Player_IsActionBlocked(struct Player *);
s32 sub_08249244(void *, void *, void *, void *);
void Sound_PlaySong(u32);
static inline u32 Test16(u16 *p, u32 mask)
{
    return *p & mask;
}
static inline u32 Test32(u32 *p, u32 mask)
{
    return *p & mask;
}
void Player_AccumulateContactCost(struct PlayerContactSource *source,
                                  struct ContactCollider *contact)
{
    struct Player *p;
    s32 i;
    if (Test16(&contact->contactFlags, 0x10)) {
        p = source->owner;
        Collision_CalculateDamageResponse(source, contact);
        if (p->actionMode == 1) {
            if (p->statModifierContext.savedActionIndex == 1 && Player_IsActionBlocked(p) &&
                !(contact->costFlags & 8)) {
                contact->cost >>= 2;
                if (contact->flags & 0x100)
                    contact->costFlags |= 8;
            }
            if ((Test32(&contact->reactionFlags, 0xF80)) && !p->contactCounted) {
                s32 total, count, threshold;
                s32 *counter;
                (*(s32 *)((u8 *)gGameState + 0x600))++;
                counter = (s32 *)((u8 *)gGameState + 0x600);
                total = *(s32 *)((u8 *)gGameState + 0x604);
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
