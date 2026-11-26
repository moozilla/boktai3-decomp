#include "global.h"
#include "engine/engine.h"
#include "npc/story_character/state_actor.h"
// confidence=low: Vampire pending-damage handler: applies queued damage, flash/shake, death and stagger transitions.
struct VampireDamage {
    u8 pad0[0x48];
    u8 sprite[0x14];
    u8 pad5C[0x34];
    u32 timer;
    s16 health, maxHealth;
    u8 pad98[2];
    u8 healthCheck, pad9b;
    u32 flags;
    u8 padA0[2];
    u8 reset;
    u8 padA3;
    u32 pending;
    u8 padA8[3];
    u8 state;
    u8 padAC[5];
    u8 changed;
    u8 padB2[0x4e];
    void (*callback)(void *);
    u8 pad104[0xc8];
    u8 collider[0x43];
    u8 contactFlags;
    u8 pad210[0x78];
    void *damageEffect;
    u8 pad28c[0xb0];
    s32 pendingDamage;
    u8 pendingSpecial;
    u8 forcedHit;
    u8 pad342[0xa];
    u16 flashTimer;
    u16 effectState;
    u16 effectCooldown;
    u8 pad352[6];
    s32 kind;
    u8 pad35c[0x1a];
    s16 hitThreshold;
    u8 pad378[0xc];
    u8 damageGuard;
    u8 pad385[0x13];
    u8 shakeAngle;
    u8 shakeAmplitude;
    u8 pad39a[0xe];
    u32 hitCount;
    u32 damageFlags;
    u8 pad3b0[0x214];
    u8 normalEffect[0x200];
    u8 specialEffect[1];
};

struct VampireDamageGameState {
    u8 pad0[0x876];
    s16 damageLevel;
};
struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void SceneActors_SpawnDestructionParticles(void *);
int SceneActorGraphics_GetEffectState(void *);
void Count_UpdateSwayReaction(void *, int);
void sub_08027214(void *, int);

/* Health is stored in the signed halfword domain. */
static inline s16 Vampire_GetHealthFloor(void)
{
    return 0;
}

int Count_ApplyPendingDamage(struct VampireDamage *p)
{
    int special;
    struct VampireDamage *base = p;
    u8 *sprite = p->sprite;
    void *collider = p->collider;
    int damage;
    int trigger;
    int flaggedBit;
    u8 *hit;
    s16 *hp;
    struct Packed size;
    {
        s32 *damagePtr = &p->pendingDamage;
        damage = *damagePtr;
        {
            u8 *specialPtr = &p->pendingSpecial;
            int value;
            value = *specialPtr;
            special = value;
            *damagePtr = 0;
            hit = &p->forcedHit;
            *hit = 0;
            *specialPtr = 0;
        }
    }
    flaggedBit = p->contactFlags >> 3;
    flaggedBit &= 1;
    if (((p->damageFlags & 1) && (p->damageGuard == 0)) &&
        (((struct VampireDamageGameState *)gGameState)->damageLevel > 0)) {
        *hit = 1;
        damage = p->health - (p->maxHealth / 2);
        if (damage < 0)
            damage = 0;
    }
    if (SceneActorState_TestFlags((struct StateActor *)base, 0x100000) && p->forcedHit == 0 &&
        damage > 0)
        p->hitCount++;
    if (p->effectCooldown == 0) {
        if ((p->effectState == 0) && SceneActorGraphics_GetEffectState(p)) {
            SceneActors_SpawnDestructionParticles(p);
            damage = ((struct VampireDamageGameState *)gGameState)->damageLevel * 5;
            Sound_PlaySongChecked(0x12a);
            p->effectCooldown = 60;
        }
    } else {
        p->effectCooldown -= 1;
    }
    if ((damage > 0) || (p->forcedHit != 0)) {
        {
            s16 *h = &base->health;
            int forced;
            *h -= damage;
            forced = p->forcedHit;
            hp = h;
            if (forced) {
                trigger = 1;
            } else if (p->hitCount >= ((u32)p->hitThreshold)) {
                trigger = 1;
                p->hitCount = 0;
            } else {
                trigger = 0;
            }
        }
        p->flashTimer = 10;
        if (special) {
            p->damageEffect = p->specialEffect;
            SceneActors_SpawnDestructionParticles(p);
        } else {
            p->damageEffect = p->normalEffect;
        }
        size.x = 16;
        size.y = 0;
        size.z = 16;
        DamageNumberDisplay_Show(collider, damage, 0, sprite + 0xc, &size, 1);
        if ((*hp) <= 0) {
            s16 health = Vampire_GetHealthFloor();
            *hp = health;
            SceneActorState_SetFlags((struct StateActor *)base, 2);
            SceneActorState_EnterAction((struct StateActor *)base, sub_08027214, 25);
            return 1;
        }
        if (p->forcedHit == 0) {
            if (special) {
                SceneActorState_SetFlags((struct StateActor *)base, 0x80000);
            }
            Sound_PlaySongChecked(0x13e);
        }
        if (p->kind == 3)
            trigger = flaggedBit;
        if (trigger) {
            SceneActorState_SetFlags((struct StateActor *)base, 1);
            SceneActorState_EnterAction((struct StateActor *)base, Count_UpdateSwayReaction, 24);
            return 1;
        }
        p->shakeAmplitude = 8;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 index = gRandomIndex * 2;
            u32 value;
            value = *((u16 *)(((u8 *)RANDOM_TABLE_ADDR) + index));
            p->shakeAngle = value;
        }
    }
    return 0;
}
