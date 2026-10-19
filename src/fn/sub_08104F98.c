
#ifdef NONMATCHING
// fuzzy: 95.7%
// foundry draft (worker best 95.7%); what differs is mostly register allocation and instruction order
#define CallbackPool_ApplyDamage          sub_08104F98
#define CallbackPool_RefreshHitState      sub_081050BC
#define CallbackPool_RefreshHealthDisplay sub_081056A0
#include "global.h"
#include "callback_pool.h"
void sub_0821FF8C(void *);
void SceneHud_SetTimedState(void *, s32, s32, u32, s32, s32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void sub_081050BC(void *);
void sub_081056A0(void *);
void Sound_PlaySong(u32);
static inline void ClearVector(void *dest, u32 *zero)
{
    *zero = 0;
    CpuSet(zero, dest, 0x05000002);
}
void sub_08104F98(struct CallbackPoolEntry *source, struct CallbackPoolEntry *target,
                  struct CallbackPoolEntry *entry)
{
    struct CallbackPoolEntry *recipient = target;
    s32 damage;
    u32 stack[3];
    u32 *vector;
    if (entry->hitFlags != 0)
        return;
    sub_0821FF8C(source);
    recipient->sprite44 = source->sprite44;
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
    DamageNumberDisplay_Show(recipient, damage, 0, &entry->position, vector, 1);
    entry->hitTimer = 3;
    entry->shakeTimer = 10;
    sub_081050BC(entry);
    sub_081056A0(entry);
    if (entry->damage)
        Sound_PlaySong(0x13e);
    else
        Sound_PlaySong(0x211);
    if (entry->health <= 0) {
        entry->callbackIndex = 4;
        entry->defeated = 1;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08104F98);
#endif
