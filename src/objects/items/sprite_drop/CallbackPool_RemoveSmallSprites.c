#include "global.h"
#include "engine/ram.h"
#include "objects/items/sprite_drop/small_callback_entry_layout.h"
#define gSmallCallbackPool gCompactParticlePool
struct SmallCallbackPool {
    u8 pad0[0x18];
    struct SmallCallbackEntry entries[16];
};
void Sprite_Remove(void *);
s32 CallbackPool_RemoveSmallSprites(struct SmallCallbackPool *pool)
{
    s32 i;
    for (i = 0; i <= 15; i++) {
        if (pool->entries[i].spriteActive) {
            Sprite_Remove(pool->entries[i].sprite);
        }
    }
    return (s32)(gCompactParticlePool = 0);
}
