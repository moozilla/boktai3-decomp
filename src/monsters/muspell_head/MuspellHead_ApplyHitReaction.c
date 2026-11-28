#include "global.h"
#include "engine/ram.h"
#include "engine/math/hit_spark_random.h"

#include "engine/math/scene_vec.h"
#include "engine/engine.h"
#include "monsters/muspell_head.h"

/* Names and field interpretations are tentative. */

extern const struct SceneVec gUnk_0824FE44;
void Collision_CalculateDamageResponse(void *attacker, void *defender);
void Sound_PlaySong(u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void PlayerStats_AddContactHundredths(void *, void *, u32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void MuspellHead_ApplyHitReaction(struct ContactCollider *source, struct ContactCollider *hit,
                                  struct MuspellHead *head)
{
    Collision_CalculateDamageResponse(source, hit);
    if (hit->responseValue) {
        hit->cost = 0;
        hit->costFlags = 0;
    } else {
        head->damage = hit->cost;
        head->currentHit = source->contactFlags;
        head->bodyCollider.responseValue = source->responseValue;
        if (source->reactionFlags & 0x40) {
            head->bodyCollider.responseValue = 0x26;
            head->flash = 8;
            Sound_PlaySong(0x211);
            DamageNumberDisplay_Show(&head->bodyCollider, head->damage, 0,
                                     &head->bodyCollider.position, &head->bodyCollider.offset, 1);
            SceneHud_SetTimedState(head->parent, head->health, head->maximumHealth, head->hudMode,
                                   head->bodyCollider.costFlags == 4, head->hudValue);
            if (source->reactionFlags & 0x40)
                goto sparks;
        }
        PlayerStats_AddContactHundredths(source, hit, 0);
    sparks:
        if (source->reactionFlags & head->damageFlags) {
            struct {
                struct SceneVec position, velocity;
            } frame;
            s32 i;
            struct SceneVec *vel = &frame.velocity;
            struct SceneVec *pos = &frame.position;
            u32 *index = &gRandomIndex;
            u32 limit = 0x3ff;
            u32 mask = 0x7f;
            for (i = 2; i >= 0; i--) {
                u32 random;
                u32 value;
                u32 sample;
                u32 x;
                frame.velocity = gUnk_0824FE44;
                frame.position = *(struct SceneVec *)&head->sprite.position;
                value = (u32)gRandomTable;
                random = HitSpark_NextIndex(*index, limit);
                sample = HitSpark_Sample(value, random);
                x = (u16)pos->x - 64;
                sample &= mask;
                x += sample;
                pos->x = x;
                random = HitSpark_NextIndex(random, limit);
                pos->y += (u8)HitSpark_Sample(value, random);
                random = HitSpark_NextIndex(random, limit);
                *index = random;
                sample = HitSpark_Sample(value, random);
                x = (u16)pos->z - 64;
                sample &= mask;
                x += sample;
                pos->z = x;
                value = 0;
                SparkParticle_Spawn(&frame.position, 1, vel, value, 3, 8, 2, 2);
            }
        }
    }
}
