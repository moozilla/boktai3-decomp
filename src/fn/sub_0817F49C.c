#include "global.h"
#include "state_actor_action_entry.h"
// confidence=low: BossSlot2 pending-damage handler: applies damage, shows numbers, picks hurt/knockback/death state.
#define H(o) (*(s16 *)(q + (o)))
#define B(o) (*(u8 *)(q + (o)))
#define W(o) (*(u32 *)(q + (o)))
struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
extern u8 *gGameState;
u32 ActorExtension_ResetState(void *);
s32 SceneActors_GetEffectiveTimeStep(void *);
void SceneActors_SpawnDestructionParticles_0817ABEC(void *);
void Sound_PlaySong(u32);
s32 ActorResource_ClampDecrease(void *, s32);
s32 ActorResource_IsBelowSelectedLimit(void *, s32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void SceneEventActor_UpdateRemovalSequence(void *);
void SceneVolume_UpdateKnockbackLanding(void *);

int BossSlot2_ApplyPendingDamage(u8 *p)
{
    struct StateActor *base = (struct StateActor *)p;
    u8 *q = p;
    u32 special;
    u8 *position = p + 0x48;
    u8 *collider = p + 0x1cc;
    u32 off = 0x46a;
    s32 damage;
    u8 *specialPtr;
    u32 flags;
    s16 *health;
    u32 sound;
    damage = *(s16 *)(q + off);
    specialPtr = q + 0x470;
    special = *specialPtr;
    off += 2;
    flags = *(u32 *)(q + off);
    ActorExtension_ResetState(base);
    *specialPtr = 0;
    off = SceneActorState_TestFlags(base, 0x100000);
    if (off) {
        damage = damage >= 0 ? damage >> 1 : -(-damage >> 1);
    }
    if (flags & 0x40) {
        special = 0;
        sound = 0x211;
    } else
        sound = 0x13e;
    {
        u16 *hitstun = (u16 *)(q + 0x4f0);
        if (*hitstun == 0) {
            if (*(u16 *)(q + 0x4ee) == 0 && !off && SceneActors_GetEffectiveTimeStep(q)) {
                damage = *(s16 *)(gGameState + 0x876) * 5;
                SceneActors_SpawnDestructionParticles_0817ABEC(q + 0x2ec);
                sound = 0x12a;
                Sound_PlaySong(0x57a);
                *hitstun = 60;
                SceneActorState_SetFlags(base, 1);
            }
        } else
            (*hitstun)--;
    }
    if (damage > 0 || ((s16)flags & 0x40)) {
        s32 knockback = 0;
        struct Packed size;
        damage = ActorResource_ClampDecrease(base, damage);
        base->health -= damage;
        Sound_PlaySong(sound);
        health = &base->health;
        if (damage > 0 && base->actionState != 0xb) {
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
            s16 clearedHealth[1];
            int zero;
            u32 callback;
            int state;
            clearedHealth[0] = 0;
            zero = 0;
            *health = clearedHealth[0];
            SceneActorState_SetFlags(base, 2);
            callback = (u32)SceneEventActor_UpdateRemovalSequence;
            state = 0xd;
            base->actionState = state;
            base->actionCallback = (void *)callback;
            base->callbackFrame = 0;
            base->actionChanged = 1;
            base->reactionPhase = zero;
            base->reactionTimer = 0;
            return 1;
        }
        if (knockback) {
            SceneActorState_SetFlags(base, 1);
            SceneActorState_StartAction(base, 1, 0, 2);
            SceneActorState_StartReaction(base, SceneVolume_UpdateKnockbackLanding, 0xc);
            return 1;
        }
    }
    return 0;
}
