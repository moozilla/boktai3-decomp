#include "global.h"
#include "callback_pool.h"

void Collision_CalculateDamageResponse(void *, void *);
void SceneHud_SetTimedState(void *, s32, s32, u32, s32, s32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void CallbackPool_SpawnHitParticles(void *);
void CallbackPool_RefreshHealthDisplay(void *);
void Sound_PlaySong(u32);
static inline void ClearVector(void *dest, u32 *zero)
{
    *zero = 0;
    CpuSet(zero, dest, 0x05000002);
}
void CallbackPool_ApplyDamage(struct CallbackPoolEntry *source, struct CallbackPoolEntry *target,
                              struct CallbackPoolEntry *entry)
{

    s32 damage;
    u32 stack[3];
    u32 *vector;
    if (entry->hitFlags != 0)
        return;
    Collision_CalculateDamageResponse(source, target);
    target->sprite44 = source->sprite44;
    damage = entry->health - entry->damage;
    if (damage < 0)
        damage += entry->damage;
    else
        damage = entry->damage;
    entry->health -= entry->damage;
    if (entry->health < 0)
        entry->health = 0;
    SceneHud_SetTimedState(entry->healthDisplay, entry->health, entry->maxHealth,
                           entry->displayMode, 0, -1);
    vector = stack + 1;
    ClearVector(vector, stack);
    DamageNumberDisplay_Show(target, damage, 0, &entry->position, vector, 1);
    entry->hitTimer = 3;
    entry->shakeTimer = 10;
    CallbackPool_SpawnHitParticles(entry);
    CallbackPool_RefreshHealthDisplay(entry);
    if (entry->damage)
        Sound_PlaySong(0x13e);
    else
        Sound_PlaySong(0x211);
    if (entry->health <= 0) {
        entry->callbackIndex = 4;
        entry->defeated = 1;
    }
}
