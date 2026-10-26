#include "global.h"
#include "ram.h"
#define gSpritePairCompact gCompactSpritePool
struct CompactSpriteSlot {
    u8 pad00[0x1c];
    u32 sprite;
    u8 active;
    u8 pad05[0x3f];
    u32 state;
};
struct CompactSpritePair {
    u8 pad00[0x18];
    struct CompactSpriteSlot slots[2];
};
void Sprite_Remove(void *);
void SpritePair_RemoveSlot(u32 index)
{
    struct CompactSpritePair *pair = gCompactSpritePool;
    if (pair && index <= 1 && pair->slots[index].active) {
        Sprite_Remove(&pair->slots[index].sprite);
        pair->slots[index].state = 0;
    }
}
