#include "global.h"
// confidence=low: BossSlot2 pending-damage handler: applies damage, shows numbers, picks hurt/knockback/death state.
#define H(o) (*(s16 *)(q + (o)))
#define B(o) (*(u8 *)(q + (o)))
#define W(o) (*(u32 *)(q + (o)))
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

struct DamageActor {
    u8 pad00[0x90];
    u32 stateValue;
    s16 health;
    u8 pad96[6];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 padA8[3];
    u8 state;
    u8 reaction;
    u8 reactionPhase;
    u8 reactionMode;
    u8 padAF;
    u8 active;
    u8 stateReady;
    u8 padB2[0x12];
    u32 reactionTimer;
    u8 padC8[0x38];
    u32 callback;
};

int BossSlot2_ApplyPendingDamage(u8 *p)
{
    struct DamageActor *base = (struct DamageActor *)p;
    u8 *q = p;
    u32 special;
    u8 *position = p + 0x48;
    u8 *collider = p + 0x1cc;
    u32 off = 0x46a;
    s32 damage;
    u8 *specialPtr;
    union {
        u32 flags;
        s16 *health;
    } pending;
    u32 sound;
    damage = *(s16 *)(q + off);
    specialPtr = q + 0x470;
    special = *specialPtr;
    off += 2;
    pending.flags = *(u32 *)(q + off);
    ActorExtension_ResetState(base);
    *specialPtr = 0;
    off = TestFlags((struct FlagsHolder *)base, 0x100000);
    if (off) {
        damage = damage >= 0 ? damage >> 1 : -(-damage >> 1);
    }
    if (pending.flags & 0x40) {
        special = 0;
        sound = 0x211;
    } else
        sound = 0x13e;
    {
        u16 *hitstun = (u16 *)(q + 0x4f0);
        if (*hitstun == 0) {
            if (*(u16 *)(q + 0x4ee) == 0 && !off && SceneActors_GetEffectiveTimeStep(q)) {
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
    if (damage > 0 || ((s16)pending.flags & 0x40)) {
        s32 knockback = 0;
        struct Packed size;
        damage = ActorResource_ClampDecrease(base, damage);
        base->health -= damage;
        Sound_PlaySong(sound);
        pending.health = &base->health;
        if (damage > 0 && base->state != 0xb) {
            B(0x475) = 0;
            B(0x474)++;
        }
        if (B(0x474) > 2) {
            B(0x474) = 0;
            knockback = 1;
        }
        if (ActorResource_IsBelowSelectedLimit(q, *pending.health))
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
        if (*pending.health <= 0) {
            s16 clearedHealth[1];
            int zero;
            u32 callback;
            int state;
            clearedHealth[0] = 0;
            zero = 0;
            *pending.health = clearedHealth[0];
            SetFlags((struct FlagsHolder *)base, 2);
            callback = (u32)SceneEventActor_UpdateRemovalSequence;
            state = 0xd;
            base->state = state;
            base->callback = callback;
            base->stateValue = 0;
            base->stateReady = 1;
            base->phase = zero;
            base->timer = 0;
            return 1;
        }
        if (knockback) {
            int zero;
            int mode;
            u32 callback;
            int state;
            SetFlags((struct FlagsHolder *)base, 1);
            zero = 0;
            mode = 2;
            base->reaction = 1;
            base->reactionPhase = zero;
            base->reactionMode = mode;
            base->reactionTimer = zero;
            base->active = 1;
            callback = (u32)SceneVolume_UpdateKnockbackLanding;
            state = 0xc;
            base->state = state;
            base->callback = callback;
            base->stateValue = zero;
            base->stateReady = 1;
            base->phase = zero;
            base->timer = zero;
            return 1;
        }
    }
    return 0;
}
