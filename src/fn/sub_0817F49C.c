#include "global.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: BossSlot2 pending-damage handler: applies damage, shows numbers, picks hurt/knockback/death state.
#define BossSlot2_ApplyPendingDamage sub_0817F49C
#define H(o)                         (*(s16 *)(q + (o)))
#define B(o)                         (*(u8 *)(q + (o)))
#define W(o)                         (*(u32 *)(q + (o)))
struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
extern u8 *gUnk_02000710;
u32 ActorExtension_ResetState(void *);
s32 SceneActors_GetEffectiveTimeStep(void *);
void SceneActors_SpawnDestructionParticles_0817ABEC(void *);
void Sound_PlaySong(u32);
s32 ActorResource_ClampDecrease(void *, s32);
s32 ActorResource_IsBelowSelectedLimit(void *, s32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void SceneEventActor_UpdateRemovalSequence(void *);
void SceneVolume_UpdateKnockbackLanding(void *);

struct FlagsHolder {
    u8 pad[0x9c];
    u32 flags;
};
static inline void SetFlags(struct FlagsHolder *f, u32 mask)
{
    f->flags |= mask;
}
static inline u32 TestFlags(struct FlagsHolder *f, u32 mask)
{
    return f->flags & mask;
}

#ifdef NONMATCHING
// fuzzy: 82.8%
int BossSlot2_ApplyPendingDamage(u8 *p)
{
    u8 *base = p;
    u8 *q = p;
    u32 special;
    u8 *position = p + 0x48;
    u8 *collider = p + 0x1cc;
    u32 off = 0x46a;
    s32 damage;
    u8 *specialPtr;
    u32 flags2;
    u32 halve;
    u32 sound;
    asm("" : "+r"(off));
    damage = *(s16 *)(q + off);
    specialPtr = q + 0x470;
    special = *specialPtr;
    off += 2;
    flags2 = *(u32 *)(q + off);
    ActorExtension_ResetState(base);
    *specialPtr = 0;
    halve = TestFlags((struct FlagsHolder *)base, 0x100000);
    if (halve) {
        damage = damage >= 0 ? damage >> 1 : -(-damage >> 1);
    }
    if (flags2 & 0x40) {
        special = 0;
        sound = 0x211;
    } else
        sound = 0x13e;
    {
        u16 *hitstun = (u16 *)(q + 0x4f0);
        if (*hitstun == 0) {
            if (*(u16 *)(q + 0x4ee) == 0 && !halve && SceneActors_GetEffectiveTimeStep(q)) {
                damage = *(s16 *)(gUnk_02000710 + 0x876) * 5;
                SceneActors_SpawnDestructionParticles_0817ABEC(q + 0x2ec);
                sound = 0x12a;
                Sound_PlaySong(0x57a);
                *hitstun = 60;
                SetFlags((struct FlagsHolder *)base, 1);
            }
        } else
            (*hitstun)--;
    }
    if (damage > 0 || (flags2 & 0x40)) {
        register s32 knockback asm("sl") = 0;
        s16 *health;
        struct Packed size;
        damage = ActorResource_ClampDecrease(base, damage);
        health = (s16 *)(base + 0x94);
        *health -= damage;
        Sound_PlaySong(sound);
        if (damage > 0 && *(u8 *)(base + 0xab) != 0xb) {
            B(0x475) = 0;
            B(0x474)++;
        }
        if (B(0x474) > 2) {
            B(0x474) = 0;
            knockback = 1;
        }
        if (ActorResource_IsBelowSelectedLimit(q, *health))
            knockback = 1;
        H(0x4ec) = 10;
        if (special) {
            W(0x314) = (u32)(q + 0xa00);
            SceneActors_SpawnDestructionParticles_0817ABEC(q + 0x2ec);
        } else
            W(0x314) = (u32)(q + 0x800);
        size.x = 16;
        size.y = 0;
        size.z = 16;
        DamageNumberDisplay_Show(collider, damage, 0, position + 0xc, &size, 1);
        if (*health <= 0) {
            int zero = 0;
            *health = 0;
            SetFlags((struct FlagsHolder *)base, 2);
            *(u8 *)(base + 0xab) = 0xd;
            W(0x100) = (u32)SceneEventActor_UpdateRemovalSequence;
            *(u32 *)(base + 0x90) = zero;
            *(u8 *)(base + 0xb1) = 1;
            *(u8 *)(base + 0xa2) = zero;
            *(u32 *)(base + 0xa4) = zero;
            return 1;
        }
        if (knockback) {
            int zero = 0;
            SetFlags((struct FlagsHolder *)base, 1);
            *(u8 *)(base + 0xac) = 1;
            *(u8 *)(base + 0xad) = zero;
            *(u8 *)(base + 0xae) = 2;
            *(u32 *)(base + 0xc4) = zero;
            *(u8 *)(base + 0xb0) = 1;
            *(u8 *)(base + 0xab) = 0xc;
            W(0x100) = (u32)SceneVolume_UpdateKnockbackLanding;
            *(u32 *)(base + 0x90) = zero;
            *(u8 *)(base + 0xb1) = 1;
            *(u8 *)(base + 0xa2) = zero;
            *(u32 *)(base + 0xa4) = zero;
            return 1;
        }
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0817F49C);
#endif
