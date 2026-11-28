#include "global.h"
#include "monsters/cannons/wind_cannon.h"
#include "engine/math/hit_spark_random.h"
#include "engine/engine.h"
extern const struct ContactPosition gUnk_0824FE8C;
void Collision_CalculateDamageResponse(void *, void *);
void Sound_PlaySong(u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void PlayerStats_AddContactHundredths(void *, void *, s32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void WindCannon_ApplyHitReaction(struct ContactCollider *source, struct ContactCollider *hit,
                                 struct FanCannon *target)
{
    struct {
        struct ContactPosition position, velocity;
    } frame;
    Collision_CalculateDamageResponse(source, hit);
    if (hit->responseValue) {
        hit->cost = 0;
        hit->costFlags = 0;
    } else {
        target->damage = hit->cost;
        target->f15c = source->contactFlags;
        target->collider.responseValue = source->responseValue;
        if (source->reactionFlags & 0x40) {
            void *hud;
            s32 health;
            s32 a, b, direction;
            target->collider.responseValue = 0x26;
            target->flash = 8;
            Sound_PlaySong(0x211);
            DamageNumberDisplay_Show(&target->collider, target->damage, 0,
                                     &target->collider.position, &target->collider.offset, 1);
            hud = (void *)target->name;
            health = target->health;
            a = target->maximumHealth;
            b = target->hudA;
            direction = 0;
            if (target->collider.costFlags == 4)
                direction = 1;
            SceneHud_SetTimedState(hud, health, a, b, direction, target->hudTime);
            if (source->reactionFlags & 0x40)
                goto sparks;
        }
        PlayerStats_AddContactHundredths(source, hit, 0);
    sparks:
        if (source->reactionFlags & (u16)target->hudB) {
            s32 i;
            struct ContactPosition *vel = &frame.velocity;
            struct ContactPosition *pos = &frame.position;
            u32 *index = &gRandomIndex;
            u32 limit = 0x3ff;
            u32 mask = 0x7f;
            for (i = 2; i >= 0; i--) {
                u32 random;
                u32 value;
                u32 sample;
                u32 x;
                frame.velocity = gUnk_0824FE8C;
                frame.position = target->sprite.position;
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
            }
        }
    }
}
