#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/items/sprite_drop.h"

s32 ActorParticles_InitCompactPool(struct SpriteDropPool *pool)
{
    s32 i;
    for (i = 0; i < SPRITE_DROP_COUNT; i++) {
        if (!AnimData_Load(pool->items[i].graphics, 0xae8e))
            return -1;
        Sprite_Init(&pool->items[i].sprite, pool->items[i].graphics, 0);
        pool->items[i].sequences = ScriptProc_Start(0x922e, 0x87e6);
        Memory_Clear(&pool->items[i].sprite.position, sizeof(pool->items[i].sprite.position));
        pool->items[i].active = 0;
        pool->items[i].sprite.flags |= 0x11;
        pool->items[i].sprite.priority = 2;
    }
    gCompactParticlePool = (u32)pool;
    return 0;
}
