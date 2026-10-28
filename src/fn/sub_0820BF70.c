#include "global.h"
#include "engine.h"
#include "ram.h"
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
    u8 p48[0xc0 - 0x48];
    u8 collider[0xc];
    struct Vec displayPosition;
    u8 pd4[0x103 - 0xd4];
    u8 direction;
    u16 reaction;
    u8 p106[0x1dc - 0x106];
    s16 damage;
    u16 flash;
    u8 p1e0[4];
    s16 health;
    s16 hudA;
    u16 pad;
    u16 sparkMask;
    s16 hudB;
    u8 p1ee[6];
    s16 hudTime;
    u8 p1f6[2];
    void *hud;
    u8 p1fc[8];
    u32 damageType;
};
extern const struct Vec gUnk_0824FE9C;
void Collision_CalculateDamageResponse(void *);
void Sound_PlaySong(u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void PlayerStats_AddContactHundredths(void *, void *, s32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void Screamer_ApplyHitReaction(struct Source *sourceInput, struct Hit *input,
                               struct Target *targetInput)
{
    struct {
        struct Vec position, velocity;
    } frame;
    struct Source *source = sourceInput;
    struct Hit *hit = input;
    struct Target *target = targetInput;
    Collision_CalculateDamageResponse(source);
    if (hit->blocked) {
        hit->damage = 0;
        hit->state = 0;
    } else {
        target->damage = hit->damage;
        target->damageType = source->type;
        target->reaction = source->reaction;
        if (source->flags & 0x40) {
            register void *hud asm("r8");
            register s32 health asm("r10");
            s32 a, b, direction;
            target->reaction = 0x26;
            target->flash = 8;
            Sound_PlaySong(0x211);
            DamageNumberDisplay_Show(target->collider, target->damage, 0, &target->displayPosition,
                                     (u8 *)target + 0xd4, 1);
            hud = target->hud;
            health = target->health;
            a = target->hudA;
            b = target->hudB;
            direction = 0;
            if (target->direction == 4)
                direction = 1;
            SceneHud_SetTimedState(hud, health, a, b, direction, target->hudTime);
            if (source->flags & 0x40)
                goto sparks;
        }
        PlayerStats_AddContactHundredths(source, hit, 0);
    sparks:
        if (source->flags & target->sparkMask) {
            register s32 i asm("r6");
            struct Vec *vel = &frame.velocity;
            struct Vec *pos = &frame.position;
            register u32 *index asm("r9") = &gRandomIndex;
            u32 mask = 0x7f;
            i = 2;
            do {
                register u32 random asm("r1");
                register const u16 *table asm("r3");
                u32 sample;
                u32 x;
                frame.velocity = gUnk_0824FE9C;
                frame.position = target->position;
                table = (const u16 *)RANDOM_TABLE_ADDR;
                {
                    register u32 *cursor asm("r2") = index;
                    random = (*cursor + 1) & 0x3ff;
                    asm("" : : "r"(cursor));
                }
                sample = table[random];
                x = pos->x - 0x40;
                sample &= mask;
                x += sample;
                pos->x = x;
                random = (random + 1) & 0x3ff;
                pos->y += (u8)table[random];
                random = (random + 1) & 0x3ff;
                {
                    register u32 *cursor asm("r7") = index;
                    *cursor = random;
                }
                {
                    u32 last = table[random];
                    x = pos->z - 0x40;
                    last &= mask;
                    x += last;
                    pos->z = x;
                }
                SparkParticle_Spawn(&frame.position, 1, vel, 0, 3, 8, 2, 2);
                i--;
            } while (i >= 0);
        }
    }
}
