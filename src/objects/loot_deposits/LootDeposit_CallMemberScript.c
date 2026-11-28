#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/loot_deposits/callback_pool.h"

void LootDeposit_CallMemberScript(struct CallbackPoolEntry *entry)
{
    s32 values[12];
    struct CallbackPoolScriptArgs arguments;
    u32 script = entry->script;
    if (script) {
        values[0] = entry->sprite.position.x;
        values[1] = entry->sprite.position.y;
        values[2] = entry->sprite.position.z;
        values[3] = entry->arguments[0];
        values[4] = entry->arguments[1];
        values[5] = entry->arguments[2];
        values[6] = -1;
        values[7] = 0;
        values[8] = 0;
        values[9] = 0;
        values[10] = 0;
        values[11] = -1;
        arguments.count = 12;
        arguments.values = values;
        Script_Call(script, &arguments);
    }
}
