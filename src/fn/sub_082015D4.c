#include "global.h"
#include "ram.h"
#include "hit_spark_random.h"

#include "scene_vec.h"
#include "engine.h"

/* Names and field interpretations are tentative. */

struct HitSource {
    u8 pad0[0xA];
    u16 damageType;
    u8 padC[0x2C];
    u32 flags;
    u8 pad3C[8];
    u16 reaction;
};
struct HitResult {
    u8 pad0[0x3E];
    u16 damage;
    u8 pad40[3];
    u8 state;
    u16 blocked;
};
struct HitActor {
    u8 pad0[0x40];
    struct SceneVec position;
    u8 pad48[0x78];
    u8 display[12];
    struct SceneVec origin;
    struct SceneVec offset;
    u8 padDC[0x27];
    u8 mode;
    u16 reaction;
    u8 pad106[0x76];
    s16 hudValue;
    u8 pad17E[2];
    s16 damage;
    u8 pad182[10];
    s16 flash;
    u8 pad18E[10];
    s16 hudX;
    u8 pad19A[2];
    u16 damageFlags;
    s16 hudY;
    u8 pad1A0[2];
    s16 hudTime;
    u8 pad1A4[8];
    void *hud;
    u8 pad1B0[8];
    u32 damageType;
};
extern const struct SceneVec gUnk_0824FE44;
void Collision_CalculateDamageResponse(void *attacker, void *defender);
void Sound_PlaySong(u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void PlayerStats_AddContactHundredths(void *, void *, u32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void ScriptCombatActor_ApplyHitReaction(struct HitSource *source, struct HitResult *hit,
                                        struct HitActor *p)
{
    Collision_CalculateDamageResponse(source, hit);
    if (hit->blocked) {
        hit->damage = 0;
        hit->state = 0;
    } else {
        p->damage = hit->damage;
        p->damageType = source->damageType;
        p->reaction = source->reaction;
        if (source->flags & 0x40) {
            p->reaction = 0x26;
            p->flash = 8;
            Sound_PlaySong(0x211);
            DamageNumberDisplay_Show(p->display, p->damage, 0, &p->origin, &p->offset, 1);
            {
                void *hud;
                s32 health, hudX, hudY, mode;
                hud = p->hud;
                health = p->hudValue;
                hudX = p->hudX;
                hudY = p->hudY;
                mode = 0;
                if (p->mode == 4)
                    mode = 1;
                SceneHud_SetTimedState(hud, health, hudX, hudY, mode, p->hudTime);
            }
            if (source->flags & 0x40)
                goto sparks;
        }
        PlayerStats_AddContactHundredths(source, hit, 0);
    sparks:
        if (source->flags & p->damageFlags) {
            struct {
                struct SceneVec position, velocity;
            } frame;
            s32 i;
            struct SceneVec *vel = &frame.velocity;
            struct SceneVec *pos = &frame.position;
            u32 *index = &gRandomIndex;
            u32 limit = 0x3ff;
            u32 mask = 0x7f;
            i = 2;
            do {
                u32 random;
                u32 value;
                u32 sample;
                u32 x;
                frame.velocity = gUnk_0824FE44;
                frame.position = p->position;
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
                {
                    u32 last = HitSpark_Sample(value, random);
                    x = (u16)pos->z - 64;
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
