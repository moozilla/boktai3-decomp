#include "global.h"
// confidence=low: shares Combat_ApplyHitReaction and emits sparks for selected damage flags.
#define ScriptCombatActor_ApplyHitReaction sub_082015D4

#ifdef NONMATCHING
// fuzzy: 70.0%
// Remaining differences: RNG stores, loop constants, target-field scheduling and allocation.
#include "scene_vec.h"

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
extern u32 gUnk_03005308;
extern const struct SceneVec gUnk_0824FE44;
void sub_0821FF8C(void *);
void Sound_PlaySong(u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void PlayerStats_AddContactHundredths(void *, void *, u32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
#define H(o) (*(s16 *)(p + (o)))
#define U(o) (*(u16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
static inline u32 RandomSample(const u16 *table)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return table[gUnk_03005308];
}
static inline void RandomizePosition(struct SceneVec *v, const u16 *table)
{
    u16 *x = (u16 *)&v->x, *y = (u16 *)&v->y, *z = (u16 *)&v->z;
    u32 sample = RandomSample(table);
    u32 base = *x - 64;
    *x = base + (sample & 127);
    *y = *y + (u8)RandomSample(table);
    sample = RandomSample(table);
    base = *z - 64;
    *z = base + (sample & 127);
}
void ScriptCombatActor_ApplyHitReaction(struct HitSource *source, struct HitResult *hit, u8 *p)
{
    sub_0821FF8C(source);
    if (hit->blocked) {
        hit->damage = 0;
        hit->state = 0;
    } else {
        H(0x180) = hit->damage;
        W(0x1b8) = source->damageType;
        U(0x104) = source->reaction;
        if (source->flags & 0x40) {
            U(0x104) = 0x26;
            H(0x18c) = 8;
            Sound_PlaySong(0x211);
            DamageNumberDisplay_Show(p + 0xc0, H(0x180), 0, p + 0xcc, p + 0xd4, 1);
            SceneHud_SetTimedState(*(void **)(p + 0x1ac), H(0x17c), H(0x198), H(0x19e),
                                   p[0x103] == 4, H(0x1a2));
            if (source->flags & 0x40)
                goto sparks;
        }
        PlayerStats_AddContactHundredths(source, hit, 0);
    sparks:
        if (U(0x19c) & source->flags) {
            struct SceneVec velocity, position;
            const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;
            s32 i = 2;
            do {
                velocity = gUnk_0824FE44;
                position = *(struct SceneVec *)(p + 0x40);
                RandomizePosition(&position, table);
                SparkParticle_Spawn(&position, 1, &velocity, 0, 3, 8, 2, 2);
            } while (--i >= 0);
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_082015D4);
#endif
