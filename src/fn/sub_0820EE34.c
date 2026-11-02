#include "damage_number_display.h"
#include "ram.h"
#include "scene_hud_display.h"
#include "hit_spark_random.h"
#include "engine.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    u16 x, y, z, pad;
};
struct OrangeSpine {
    u8 p0[0x40];
    struct Vec position;
    u8 p48[0x6c - 0x48];
    union {
        struct ContactCollider contact;
        struct {
            u8 prefix[0xc];
            struct ContactPosition position, secondaryPosition;
        } display;
    } collider;
    u8 unknownC0[0x126 - 0xc0];
    s16 health;
    s16 damage;
    u8 p12a[0x130 - 0x12a];
    u16 flash;
    u8 p132[0x138 - 0x132];
    s16 hudA;
    u8 p13a[0x13e - 0x13a];
    u16 sparkMask;
    s16 hudB;
    u8 p142[0x148 - 0x142];
    s16 hudTime;
    u8 p14a[2];
    void *hud;
    u8 p150[4];
    u32 damageType;
};
extern const struct Vec gUnk_0824FEAC;
void Collision_CalculateDamageResponse(void *attacker, void *defender);
void Sound_PlaySong(u32);
void PlayerStats_AddContactHundredths(void *, void *, s32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);

void OrangeSpine_ApplyHitReaction(struct ContactCollider *sourceInput,
                                  struct ContactCollider *input, struct OrangeSpine *targetInput)
{
    struct {
        struct Vec position, velocity;
    } frame;
    struct ContactCollider *source = sourceInput;
    struct ContactCollider *hit = input;
    struct OrangeSpine *target = targetInput;
    u32 m40, hitFlags;
    Collision_CalculateDamageResponse(source, hit);
    if (hit->responseValue) {
        hit->cost = 0;
        hit->costFlags = 0;
    } else {
        target->damage = hit->cost;
        target->damageType = source->contactFlags;
        {
            u16 reaction;
            reaction = source->responseValue;
            {
                register u32 d asm("r0");
                asm("" : "=r"(d));
                target->collider.contact.responseValue = reaction;
                asm("" : : "r"(d));
            }
        }
        hitFlags = source->reactionFlags;
        m40 = 0x40;
        if (hitFlags & m40) {
            target->flash = 8;
            Sound_PlaySong(0x211);
            {
                s32 damage;
                struct DamageNumberSource *col = (void *)&target->collider.contact;
                damage = target->damage;
                DamageNumberDisplay_Show(col, damage, 0, &target->collider.display.position,
                                         &target->collider.display.secondaryPosition, 1);
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
                    a = target->hudA;
                    asm("" : : "r"(d));
                }
                b = target->hudB;
                direction = 0;
                if (target->collider.contact.costFlags == 4)
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
            struct Vec *vel = &frame.velocity;
            struct Vec *pos = &frame.position;
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
                frame.position = target->position;
                value = (u32)gRandomTable;
                random = HitSpark_NextIndex(*index, limit);
                sample = HitSpark_Sample(value, random);
                x = pos->x - 0x40;
                sample &= mask;
                x += sample;
                pos->x = x;
                random = HitSpark_NextIndex(random, limit);
                pos->y += (u8)HitSpark_Sample(value, random);
                random = HitSpark_NextIndex(random, limit);
                *index = random;
                {
                    u32 last = HitSpark_Sample(value, random);
                    x = pos->z - 0x40;
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
