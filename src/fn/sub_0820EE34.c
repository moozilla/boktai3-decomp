#include "global.h"
#include "ram.h"
#include "hit_spark_random.h"
#include "engine.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    u16 x, y, z, pad;
};
struct Source {
    u8 p0[0xa];
    u16 type;
    u8 pc[0x38 - 0xc];
    u32 flags;
    u8 p3c[8];
    u16 reaction;
};
struct Hit {
    u8 p0[0x3e];
    u16 damage;
    u8 p40[3];
    u8 state;
    u16 blocked;
};
struct Target {
    u8 p0[0x40];
    struct Vec position;
    u8 p48[0x6c - 0x48];
    u8 collider[0xc];
    struct Vec displayPosition;
    u8 p80[0xaf - 0x80];
    u8 direction;
    u16 reaction;
    u8 pb2[0x126 - 0xb2];
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
void Collision_CalculateDamageResponse(void *);
void Sound_PlaySong(u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void PlayerStats_AddContactHundredths(void *, void *, s32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);

void OrangeSpine_ApplyHitReaction(struct Source *sourceInput, struct Hit *input,
                                  struct Target *targetInput)
{
    struct {
        struct Vec position, velocity;
    } frame;
    struct Source *source = sourceInput;
    struct Hit *hit = input;
    struct Target *target = targetInput;
    u32 m40, hitFlags;
    Collision_CalculateDamageResponse(source);
    if (hit->blocked) {
        hit->damage = 0;
        hit->state = 0;
    } else {
        target->damage = hit->damage;
        target->damageType = source->type;
        {
            u16 reaction;
            reaction = source->reaction;
            {
                register u32 d asm("r0");
                asm("" : "=r"(d));
                target->reaction = reaction;
                asm("" : : "r"(d));
            }
        }
        hitFlags = source->flags;
        m40 = 0x40;
        if (hitFlags & m40) {
            target->flash = 8;
            Sound_PlaySong(0x211);
            {
                s32 damage;
                void *col = target->collider;
                damage = target->damage;
                DamageNumberDisplay_Show(col, damage, 0, &target->displayPosition,
                                         (u8 *)target + 0x80, 1);
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
                if (target->direction == 4)
                    direction = 1;
                SceneHud_SetTimedState(hud, health, a, b, direction, target->hudTime);
            }
            if (source->flags & m40)
                goto sparks;
        }
        PlayerStats_AddContactHundredths(source, hit, 0);
    sparks:
        if (source->flags & target->sparkMask) {
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
