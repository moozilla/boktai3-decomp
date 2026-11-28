#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/video/display_sequence_step.h"
#include "objects/loot_deposits/callback_pool.h"

s32 ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
s32 Div(s32, s32);
static inline s32 ReadOptional(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}
// confidence=low: script options initialize a member of Pool184's actor group.
void LootDeposit_CreateFromScript(void)
{
    struct CallbackPool *pool = gActorGroupPool;
    struct CallbackPoolEntry *entry;
    struct DisplaySequenceSprite *render;
    s32 a, b, c;
    s32 i;
    s32 range;
    if (!pool) {
        pool = LootDepositPool_CreateManager();
        if (!pool)
            return;
    }
    entry = LootDepositPool_Allocate(pool);
    if (!entry)
        return;
    render = &entry->sprite;

    AnimData_Load(&entry->graphics, 0x9385);
    Sprite_Init(entry, &entry->graphics, 0);
    entry->defeated = 1;
    entry->spawnIndex = 0;
    entry->statusFlags = Script_GetKeywordValue('m', 0);
    if (LootDepositPool_TestStatusFlags(entry, 2))
        entry->callbackIndex = 2;
    else
        entry->callbackIndex = 1;
    if (Script_SeekToKeyword('p')) {
        render->position.x = Script_GetValue();
        render->position.y = Script_GetValue();
        render->position.z = Script_GetValue();
    }
    entry->savedPosition = render->position;
    entry->animationRow = Script_GetKeywordValue('r', 0);
    entry->healthDisplay = (void *)Script_GetKeywordValue('n', 0);
    range = Script_GetKeywordValue('a', 256);
    entry->triggerDistanceSquared = range * range;
    entry->spawnTarget = Script_GetKeywordValue('b', 0);
    if (Script_SeekToKeyword('A')) {
        entry->health = ReadOptional();
        entry->displayMode = ReadOptional();
        entry->unknown150 = ReadOptional();
        a = ReadOptional();
        b = ReadOptional();
        c = ReadOptional();
    } else {
        entry->health = 100;
        entry->displayMode = 3;
        entry->unknown150 = 0;
        a = 0;
        b = 0;
        c = 0;
    }
    entry->maxHealth = entry->health;
    entry->damageThreshold = Div(entry->health, entry->spawnTarget - 1);
    if (Script_SeekToKeyword('P')) {
        u32 *dest = &entry->spawnScript;
        *dest = ReadOptional();
    }
    if (LootDepositPool_TestStatusFlags(entry, 2))
        entry->effectIndex = ScaledShadow_Initialize(&entry->shadow, &entry->sprite.position, 64,
                                                     16, 96, 100, 2560, 1, 5, 1, 0);
    else
        entry->effectIndex = ScaledShadow_Initialize(&entry->shadow, &entry->sprite.position, 64,
                                                     16, 128, 100, 2560, 1, 5, 1, 0);
    entry->script = Script_GetKeywordValue('I', 0);
    if (entry->script) {
        Memory_Clear(entry->arguments, sizeof(entry->arguments));
        if (Script_SeekToKeyword('G')) {
            i = 0;
            do {
                u32 offset = i * 4;
                s32 *base = entry->arguments;
                s32 *dest = (s32 *)((u32)base + offset);
                *dest = ReadOptional();
            } while (++i <= 2);
        }
    }
    Anim_SetSequence(&entry->sequence, pool->sequenceResource, 0, 0, entry->animationRow);

    DisplaySequence_StepSprite(render, &entry->sequence);
    LootDepositCallbackPool_InitializeCollider(entry, a, b, c);
}
