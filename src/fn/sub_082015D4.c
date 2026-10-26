#include "global.h"

// MATCHING_CONCESSION: register-pinning
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
void Collision_CalculateDamageResponse(void *);
void Sound_PlaySong(u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void PlayerStats_AddContactHundredths(void *, void *, u32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
static inline void AdjustCoordinate(u16 *coordinate, u32 sample)
{
    u32 base = *coordinate - 64;
    *coordinate = base + (sample & 127);
}
static inline void RandomizePosition(struct SceneVec *v, const u16 *table)
{
    u16 *x = (u16 *)&v->x, *y = (u16 *)&v->y, *z = (u16 *)&v->z;
    register u32 index asm("r1");
    u32 sample, base;
    index = gRandomIndex;
    index++;
    index &= 1023;
    sample = table[index];
    base = *x - 64;
    *x = base + (sample & 127);
    index = (index + 1) & 1023;
    *y = *y + (u8)table[index];
    index = (index + 1) & 1023;
    gRandomIndex = index;
    AdjustCoordinate(z, table[index]);
}
void ScriptCombatActor_ApplyHitReaction(struct HitSource *source, struct HitResult *hit,
                                        struct HitActor *p)
{
    Collision_CalculateDamageResponse(source);
    if (hit->blocked) {
        register u32 zero asm("r0") = 0;
        hit->damage = 0;
        hit->state = zero;
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
                register void *hud asm("r8") = p->hud;
                SceneHud_SetTimedState(hud, p->hudValue, p->hudX, p->hudY, p->mode == 4,
                                       p->hudTime);
            }
            if (source->flags & 0x40)
                goto sparks;
        }
        PlayerStats_AddContactHundredths(source, hit, 0);
    sparks:
        if (source->flags & p->damageFlags) {
            struct SceneVec velocity, position;
            s32 i;
            for (i = 0; i < 3; ++i) {
                register const u16 *table asm("r3");
                velocity = gUnk_0824FE44;
                position = p->position;
                table = (const u16 *)RANDOM_TABLE_ADDR;
                RandomizePosition(&position, table);
                SparkParticle_Spawn(&position, 1, &velocity, 0, 3, 8, 2, 2);
            }
        }
    }
}
