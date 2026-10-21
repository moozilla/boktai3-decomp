// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: collision callback records damage and emits sparks for selected source flags.
#ifdef NONMATCHING
// fuzzy: 92.8%
// source/hit land in r6/r4 instead of r4/r6, and the first RNG-index read
// uses r2 instead of r7 for the cursor.
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
    u16 pad;
    u16 sparkMask;
    s16 hudB;
    u8 p140[2];
    s16 hudTime;
    u8 p144[0x15c - 0x144];
    void *hud;
    u8 p160[4];
    u32 damageType;
};
extern const struct Vec gUnk_0824FE4C;
extern u32 gUnk_03005308;
extern const u16 gUnk_0203B400[];
void Collision_CalculateDamageResponse(void *);
void Sound_PlaySong(u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void PlayerStats_AddContactHundredths(void *, void *, s32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
#define EnemyF426_ApplyHitReaction sub_082038A8
void EnemyF426_ApplyHitReaction(struct Source *sourceInput, struct Hit *input,
                                struct Target *targetInput)
{
    struct Hit *hitArg;
    struct {
        struct Vec position, velocity;
    } frame;
    struct Source *source = sourceInput;
    struct Hit *hit = input;
    struct Target *target = targetInput;
    Collision_CalculateDamageResponse(source);
    hitArg = hit;
    if (hit->blocked) {
        hit->damage = 0;
        hit->state = 0;
    } else {
        target->damage = hit->damage;
        target->damageType = source->type;
        target->reaction = source->reaction;
        if (source->flags & 0x40) {
            register void *hud asm("r8");
            s32 health;
            s32 a, b, direction;
            target->reaction = 0x26;
            target->flash = 8;
            Sound_PlaySong(0x211);
            DamageNumberDisplay_Show(target->collider, target->damage, 0, &target->displayPosition,
                                     (u8 *)target + 0x80, 1);
            hud = target->hud;
            health = target->health;
            a = target->hudA;
            b = target->hudB;
            direction = 0;
            if (target->direction == 4)
                direction = 1;
            SceneHud_SetTimedState(hud, health, a, b, direction, target->hudTime);
            if (sourceInput->flags & 0x40)
                goto sparks;
        }
        PlayerStats_AddContactHundredths(sourceInput, hitArg, 0);
    sparks:
        if (sourceInput->flags & target->sparkMask) {
            register s32 i asm("r6");
            struct Vec *vel = &frame.velocity;
            struct Vec *pos = &frame.position;
            register u32 *index asm("r9") = &gUnk_03005308;
            u32 mask = 0x7f;
            i = 2;
            do {
                u32 sample;
                register u32 random asm("r1");
                register const u16 *table asm("r3");
                u32 x;
                frame.velocity = gUnk_0824FE4C;
                frame.position = target->position;
                table = (const u16 *)RANDOM_TABLE_ADDR;
                {
                    u32 *cursor = index;
                    register u32 wrap asm("r0");
                    random = *cursor + 1;
                    wrap = 0x3ff;
                    asm("" : "+r"(wrap) : "r"(cursor));
                    random &= wrap;
                }
                sample = table[random];
                x = pos->x - 0x40;
                sample &= mask;
                x += sample;
                pos->x = x;
                random = (random + 1) & 0x3ff;
                pos->y += (u8)table[random];
                random = random + 1;
                random = random & 0x3ff;
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

#else
INCLUDE_ASM("asm/nonmatching", sub_082038A8);
#endif
