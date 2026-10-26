#include "global.h"
#include "ram.h"
struct GroupSprite {
    u8 unknown[4];
    u8 active;
    u8 unknown05[0x27];
};
struct SpriteGroup {
    struct GroupSprite sprites[4];
    u8 unknownB0[0xB0];
};
struct SpriteGroupPool {
    u8 unknown[0x38];
    u32 activeMask;
    struct SpriteGroup groups[4];
};
void Sprite_Remove(void *);
static inline u8 IsActive(u32 *mask, s32 slot)
{
    if (*mask & (1 << slot))
        return TRUE;
    return FALSE;
}
struct SpriteGroupPool *SpriteGroupPool_Destroy(struct SpriteGroupPool *pool)
{
    s32 i = 0;
    for (; i < 4; i++) {
        if (IsActive(&pool->activeMask, i)) {
            struct SpriteGroup *group = &pool->groups[i];
            s32 j;
            for (j = 0; j < 4; j++) {
                struct GroupSprite *sprite = &group->sprites[j];
                if (sprite->active)
                    Sprite_Remove(sprite);
            }
        }
    }
    return gSpriteGroupPool = 0;
}
