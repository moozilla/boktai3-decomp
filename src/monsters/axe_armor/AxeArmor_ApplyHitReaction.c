#include "monsters/axe_armor/axe_armor.h"
#include "engine/math/hit_spark_random.h"
#include "engine/engine.h"
extern const struct ContactPosition gUnk_0824FE54;
void Collision_CalculateDamageResponse(void *, void *);
void Sound_PlaySong(u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void PlayerStats_AddContactHundredths(void *, void *, s32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void AxeArmor_ApplyHitReaction(struct ContactCollider *sourceInput, struct ContactCollider *input,
                                struct AxeArmor *targetInput)
{
    struct {
        struct ContactPosition position;
        struct ContactPosition velocity;
    } frame;
    struct ContactCollider *source = sourceInput;
    struct ContactCollider *hit = input;
    struct AxeArmor *target = targetInput;
    Collision_CalculateDamageResponse(source, hit);
    if (hit->responseValue) {
        hit->cost = 0;
        hit->costFlags = 0;
    } else {
        target->pendingDamage = hit->cost;
        target->damageType = source->contactFlags;
        target->collider.responseValue = source->responseValue;
        if (source->reactionFlags & 0x40) {
            void *hud;
            s32 health;
            s32 a, b, direction;
            target->collider.responseValue = 0x26;
            target->flashTimer = 8;
            Sound_PlaySong(0x211);
            DamageNumberDisplay_Show(&target->collider, target->pendingDamage, 0,
                                     &target->collider.position,
                                     &target->collider.offset, 1);
            hud = target->parent;
            health = target->health;
            a = target->maxHealth;
            b = target->hudStyle;
            direction = 0;
            if (target->collider.costFlags == 4)
                direction = 1;
            SceneHud_SetTimedState(hud, health, a, b, direction, target->hudTime);
            if (source->reactionFlags & 0x40)
                goto sparks;
        }
        PlayerStats_AddContactHundredths(source, hit, 0);
    sparks:
        if (source->reactionFlags & (u16)target->rangeB) {
            s32 i;
            struct ContactPosition *vel = &frame.velocity;
            struct ContactPosition *pos = &frame.position;
            u32 *index = &gRandomIndex;
            u32 limit = 0x3ff;
            for (i = 0; i < 3; i++) {
                u32 random;
                u32 value;
                u32 sample;
                u32 x;
                frame.velocity = gUnk_0824FE54;
                frame.position = target->sprite.position;
                value = (u32)gRandomTable;
                random = HitSpark_NextIndex(*index, limit);
                sample = HitSpark_Sample(value, random);
                x = (u16)pos->x - 0x40;
                sample &= 0x7F;
                x += sample;
                pos->x = x;
                random = HitSpark_NextIndex(random, limit);
                pos->y += (u8)HitSpark_Sample(value, random);
                random = HitSpark_NextIndex(random, limit);
                *index = random;
                {
                    u32 last = HitSpark_Sample(value, random);
                    x = (u16)pos->z - 0x40;
                    last &= 0x7F;
                    x += last;
                    pos->z = x;
                }
                value = 0;
                SparkParticle_Spawn(&frame.position, 1, vel, value, 3, 8, 2, 2);
            }
        }
    }
}
