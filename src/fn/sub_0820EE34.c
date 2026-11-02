#include "orange_spine.h"
#include "collision_response.h"
#include "damage_number_display.h"
#include "ram.h"
#include "scene_hud_display.h"
#include "hit_spark_random.h"
#include "engine.h"
// MATCHING_CONCESSION: register-pinning
extern const struct ContactPosition gUnk_0824FEAC;
void Sound_PlaySong(u32);
void PlayerStats_AddContactHundredths(void *, void *, s32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);

void OrangeSpine_ApplyHitReaction(struct ContactCollider *sourceInput,
                                  struct ContactCollider *input,
                                  struct OrangeSpineActor *targetInput)
{
    struct {
        struct ContactPosition position, velocity;
    } frame;
    struct ContactCollider *source = sourceInput;
    struct ContactCollider *hit = input;
    struct OrangeSpineActor *target = targetInput;
    u32 m40, hitFlags;
    Collision_CalculateDamageResponse(source, hit);
    if (hit->responseValue) {
        hit->cost = 0;
        hit->costFlags = 0;
    } else {
        target->damage = hit->cost;
        target->pendingReaction = source->contactFlags;
        {
            u16 reaction;
            reaction = source->responseValue;
            {
                register u32 d asm("r0");
                asm("" : "=r"(d));
                target->collider.responseValue = reaction;
                asm("" : : "r"(d));
            }
        }
        hitFlags = source->reactionFlags;
        m40 = 0x40;
        if (hitFlags & m40) {
            target->flashTimer = 8;
            Sound_PlaySong(0x211);
            {
                s32 damage;
                struct DamageNumberSource *col = (void *)&target->collider;
                damage = target->damage;
                DamageNumberDisplay_Show(col, damage, 0, &target->collider.position,
                                         &target->collider.offset, 1);
            }
            {
                void *hud;
                s32 health;
                s32 a, b, direction;
                hud = target->hud;
                health = target->health;
                {
                    register u32 d asm("r1");
                    asm("" : "=r"(d));
                    a = target->maxHealth;
                    asm("" : : "r"(d));
                }
                b = target->hudStyle;
                direction = 0;
                if (target->collider.costFlags == 4)
                    direction = 1;
                SceneHud_SetTimedState((u32)hud, health, a, b, direction, target->hudTime);
            }
            if (source->reactionFlags & m40)
                goto sparks;
        }
        PlayerStats_AddContactHundredths(source, hit, 0);
    sparks:
        if (source->reactionFlags & target->sparkMask) {
            s32 i;
            struct ContactPosition *vel = &frame.velocity;
            struct ContactPosition *pos = &frame.position;
            u32 *index = &gRandomIndex;
            u32 limit = 0x3ff;
            u32 mask = 0x7f;
            i = 2;
            do {
                u32 random;
                u32 value;
                u32 sample;
                u32 x;
                frame.velocity = gUnk_0824FEAC;
                frame.position = target->sprite.display.position;
                value = (u32)gRandomTable;
                random = HitSpark_NextIndex(*index, limit);
                sample = HitSpark_Sample(value, random);
                x = (u16)pos->x - 0x40;
                sample &= mask;
                x += sample;
                pos->x = x;
                random = HitSpark_NextIndex(random, limit);
                pos->y += (u8)HitSpark_Sample(value, random);
                random = HitSpark_NextIndex(random, limit);
                *index = random;
                {
                    u32 last = HitSpark_Sample(value, random);
                    x = (u16)pos->z - 0x40;
                    last &= mask;
                    x += last;
                    pos->z = x;
                }
                value = 0;
                SparkParticle_Spawn(&frame.position, 1, vel, value, 3, 8, 2, 2);
                i--;
            } while (i >= 0);
        }
    }
}
