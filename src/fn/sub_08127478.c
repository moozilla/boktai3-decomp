#include "global.h"
#include "ram.h"
struct PooledSprite {
    u8 unknown0[0xBC];
    u16 timer, state;
    u8 active;
    s8 slot;
    u8 unknownC2[6];
    u8 reset, unknownC9;
    u16 counter;
};
struct PooledSpriteManager {
    u8 unknown0[0x860];
    u32 allocated;
};
u8 Sprite_BlinkUntilExpired(void *, u32, s32, s32);
void Sprite_Remove(void *);
static inline void release(struct PooledSpriteManager *manager, s32 slot)
{
    manager->allocated &= ~(1 << slot);
}
void PooledSprite_UpdateRemoval(void *unused, struct PooledSprite *sprite)
{
    if (sprite->reset) {
        sprite->reset = 0;
        sprite->timer = 0;
    }
    if (Sprite_BlinkUntilExpired(sprite, sprite->timer, 60, 40)) {
        struct PooledSpriteManager *manager = gPooledSpriteManager;
        s32 slot = sprite->slot;
        release(manager, slot);
        sprite->active = 0;
        Sprite_Remove(sprite);
        sprite->slot = -1;
        sprite->state = 0;
        sprite->counter = 0;
    } else
        sprite->timer++;
}
