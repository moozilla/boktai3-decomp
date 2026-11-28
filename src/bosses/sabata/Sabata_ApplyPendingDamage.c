#include "global.h"
#include "bosses/state_actor_action_entry.h"
#include "bosses/sabata/sabata_actor_state.h"

struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
extern u8 *gGameState;
u32 ActorExtension_ResetState(void *);
s32 SceneActors_GetEffectiveTimeStep(void *);
void Sabata_SpawnDestructionParticles(void *);
void Sound_PlaySong(u32);
s32 ActorResource_ClampDecrease(void *, s32);
s32 ActorResource_IsBelowSelectedLimit(void *, s32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void Sabata_UpdateRemovalSequence(void *);
void Sabata_UpdateKnockbackLanding(void *, int);

int Sabata_ApplyPendingDamage(struct SabataActorState *p)
{
    struct StateActor *base = (struct StateActor *)p;
    u32 special;
    struct SceneActorBody *body = &p->body;
    struct ContactCollider *collider = &p->bodyCollider;
    u32 flagsActive;
    s32 damage;
    u8 *specialPtr;
    u32 flags;
    s16 *health;
    u32 sound;
    damage = (s16)p->lastDamage;
    specialPtr = &p->slowDown;
    special = *specialPtr;
    flags = p->hitAttributes;
    ActorExtension_ResetState(base);
    *specialPtr = 0;
    flagsActive = SceneActorState_TestFlags(base, 0x100000);
    if (flagsActive) {
        damage = damage >= 0 ? damage >> 1 : -(-damage >> 1);
    }
    if (flags & 0x40) {
        special = 0;
        sound = 0x211;
    } else
        sound = 0x13e;
    {
        u16 *hitstun = &p->hitstun;
        if (*hitstun == 0) {
            if (p->contactHoldoff == 0 && !flagsActive && SceneActors_GetEffectiveTimeStep(p)) {
                damage = *(s16 *)(gGameState + 0x876) * 5;
                Sabata_SpawnDestructionParticles(&p->graphics.position);
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
            p->hitSequence = 0;
            p->recentHitCount++;
        }
        if (p->recentHitCount > 2) {
            p->recentHitCount = 0;
            knockback = 1;
        }
        if (ActorResource_IsBelowSelectedLimit(p, *health))
            knockback = 1;
        p->paletteRestoreTimer = 10;
        if (special) {
            p->graphics.palette = p->effectPalettes[2];
            Sabata_SpawnDestructionParticles(&p->graphics.position);
        } else
            p->graphics.palette = p->effectPalettes[1];
        size.x = 16;
        size.y = 0;
        size.z = 16;
        DamageNumberDisplay_Show(collider, damage, 0, &body->position, &size, 1);
        if (*health <= 0) {
            s16 clearedHealth[1];
            int zero;
            u32 callback;
            int state;
            clearedHealth[0] = 0;
            zero = 0;
            *health = clearedHealth[0];
            SceneActorState_SetFlags(base, 2);
            callback = (u32)Sabata_UpdateRemovalSequence;
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
            SceneActorState_EnterAction(base, Sabata_UpdateKnockbackLanding, 0xc);
            return 1;
        }
    }
    return 0;
}
