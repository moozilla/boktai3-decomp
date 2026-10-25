#include "global.h"

struct HalfVector {
    s16 x, y, z, pad;
};

/* Tentative layout of a Vanargand half's damage and state fields. */
struct PairedSceneHalf {
    u8 unk_00[0x1C];
    struct HalfVector position;
    u8 unk_24[0x38 - 0x24];
    void *palette;
    u8 unk_3C[0x108 - 0x3C];
    u8 damageCollider[0x160 - 0x108];
    u32 flags;
    u32 stateCounter;
    u32 animationCounter;
    u8 state;
    u8 requestedState;
    u8 stateEntered;
    u8 unk_16F;
    u8 transitionReady;
    u8 unk_171[3];
    s16 health;
    s16 maxHealth;
    u32 stateTimer;
    u32 hitFlags;
    u16 damage;
    u16 specialDamage;
    u16 flashTimer;
    u8 unk_186[0x194 - 0x186];
    void *hitPalette;
    u8 unk_198[4];
    u32 shake;
};

void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
s32 Player_IsSlotInActionTwelveOrThirtyEight(s32);

static inline void SetFlags(struct PairedSceneHalf *actor, u32 mask)
{
    actor->flags |= mask;
}

static inline void EnterState(struct PairedSceneHalf *actor, s32 state)
{
    u32 mask = 0x100;
    u32 blocked = actor->flags & mask;
    u8 current;
    if (blocked)
        return;
    current = actor->requestedState - 0x19;
    if (current <= 1) {
        if (!actor->transitionReady)
            return;
    }
    actor->requestedState = state;
    actor->state = state;
    actor->stateEntered = 1;
    actor->transitionReady = blocked;
    actor->stateTimer = blocked;
    actor->stateCounter = blocked;
    actor->animationCounter = blocked;
}

void sub_081AF7F4(struct PairedSceneHalf *actor)
{
    s32 damage = actor->damage + actor->specialDamage;
    u32 hit = (actor->hitFlags >> 6) & 1;
    void *collider = &actor->damageCollider;
    u32 special;
    struct HalfVector size;
    if (actor->specialDamage)
        special = 1;
    else
        special = 0;
    actor->specialDamage = 0;
    actor->damage = 0;
    actor->hitFlags = 0;
    if (damage > 0 || hit) {
        actor->health -= damage;
        {
            struct HalfVector *v = &size;
            v->x = 16;
            v->y = 0;
            v->z = 16;
        }
        DamageNumberDisplay_Show(collider, damage, 0, &actor->position, &size, 1);
        Sound_PlaySong(hit ? 0x211 : 0x13E);
        actor->flashTimer = 6;
        if (!hit)
            actor->shake += 20;
        CpuSet(actor->hitPalette, actor->palette, 0x04000008);
        if (actor->health <= 0) {
            EnterState(actor, 0x19);
            if (Player_IsSlotInActionTwelveOrThirtyEight(0))
                SetFlags(actor, 0x40);
        } else if (!special) {
            if (actor->requestedState == 9)
                EnterState(actor, 0x13);
            else if (actor->shake > 29)
                EnterState(actor, 0x16);
        }
    }
}
