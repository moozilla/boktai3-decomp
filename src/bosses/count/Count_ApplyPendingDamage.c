#include "bosses/count/count.h"
#include "engine/engine.h"
#include "npc/story_character/state_actor.h"
#include "scene/collision/packed_contact_vector.h"
#include "system/game_state/game_state.h"
// confidence=low: Count pending-damage handler: applies queued damage, flash/shake, death and stagger transitions.
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void SceneActors_SpawnDestructionParticles(void *);
int SceneActorGraphics_GetEffectState(void *);
void Count_UpdateSwayReaction(void *, int);
void sub_08027214(void *, int);

/* Health is stored in the signed halfword domain. */
static inline s16 Count_GetHealthFloor(void)
{
    return 0;
}

int Count_ApplyPendingDamage(struct Count *p)
{
    int special;
    struct Count *base = p;
    struct SceneActorBody *body = &p->body;
    struct ContactCollider *collider = &p->overlayCollider;
    int damage;
    int trigger;
    int flaggedBit;
    s16 *hp;
    struct PackedContactVector size;
    {
        s32 *damagePtr = &p->pendingDamage;
        damage = *damagePtr;
        {
            u8 *specialPtr = &p->pendingSpecial;
            special = *specialPtr;
            *damagePtr = 0;
            p->forcedHit = 0;
            *specialPtr = 0;
        }
    }
    flaggedBit = p->overlayCollider.costFlags >> 3;
    flaggedBit &= 1;
    if (((p->damageFlags & 1) && !p->damageGuard) && (gGameState->unk876 > 0)) {
        p->forcedHit = 1;
        damage = p->health - (p->maxHealth / 2);
        if (damage < 0)
            damage = 0;
    }
    if (SceneActorState_TestFlags((struct StateActor *)base, 0x100000) && !p->forcedHit &&
        damage > 0)
        p->hitCount++;
    if (!p->effectCooldown) {
        if (!p->invulnerabilityTimer && SceneActorGraphics_GetEffectState(p)) {
            SceneActors_SpawnDestructionParticles(p);
            damage = gGameState->unk876 * 5;
            Sound_PlaySongChecked(0x12a);
            p->effectCooldown = 60;
        }
    } else {
        p->effectCooldown--;
    }
    if ((damage > 0) || p->forcedHit) {
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
        p->paletteRestoreTimer = 10;
        if (special) {
            p->animation.palette = p->paletteSpecial;
            SceneActors_SpawnDestructionParticles(p);
        } else {
            p->animation.palette = p->paletteNormal;
        }
        size.x = 16;
        size.y = 0;
        size.z = 16;
        DamageNumberDisplay_Show(collider, damage, 0, &body->position, &size, 1);
        if ((*hp) <= 0) {
            s16 health = Count_GetHealthFloor();
            *hp = health;
            SceneActorState_SetFlags((struct StateActor *)base, 2);
            SceneActorState_EnterAction((struct StateActor *)base, sub_08027214, 25);
            return 1;
        }
        if (!p->forcedHit) {
            if (special) {
                SceneActorState_SetFlags((struct StateActor *)base, 0x80000);
            }
            Sound_PlaySongChecked(0x13e);
        }
        if (p->graphicsKind == 3)
            trigger = flaggedBit;
        if (trigger) {
            SceneActorState_SetFlags((struct StateActor *)base, 1);
            SceneActorState_EnterAction((struct StateActor *)base, Count_UpdateSwayReaction, 24);
            return 1;
        }
        p->shakeAmplitude = 8;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        p->shakeAngle = *(u16 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1));
    }
    return 0;
}
