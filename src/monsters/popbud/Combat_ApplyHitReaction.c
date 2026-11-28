#include "global.h"
#include "monsters/popbud/popbud_playback.h"
/* Names and field interpretations are tentative. */
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void Sound_PlaySong(u32);
void PlayerStats_AddContactHundredths(void *, void *, u32);
void Collision_CalculateDamageResponse(void *, void *);

void Combat_ApplyHitReaction(struct ContactCollider *source, struct ContactCollider *hit,
                             struct PinkSpineState *popbud)
{
    Collision_CalculateDamageResponse(source, hit);
    if (hit->responseValue) {
        hit->cost = 0;
        hit->costFlags = 0;
    } else {
        popbud->f130 = hit->cost;
        popbud->f148 = source->contactFlags;
        popbud->collider.responseValue = source->responseValue;
        if (source->reactionFlags & 0x40) {
            popbud->collider.responseValue = 0x26;
            popbud->f12C = 8;
            Sound_PlaySong(0x211);
            DamageNumberDisplay_Show(&popbud->collider, popbud->f130, 0, &popbud->collider.position,
                                     &popbud->collider.offset, 1);
            if (source->reactionFlags & 0x40)
                return;
        }
        PlayerStats_AddContactHundredths(source, hit, 0);
    }
}
