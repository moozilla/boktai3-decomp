#include "global.h"
#define SpritePair_RemoveSlot sub_0813A91C
#define gSpritePairCompact gUnk_020004D8
struct CompactSpriteSlot {
    u8 pad00[0x1c];
    u32 sprite;
    u8 active;
    u8 pad05[0x3f];
    u32 state;
    
};
struct CompactSpritePair { u8 pad00[0x18]; struct CompactSpriteSlot slots[2]; };
extern struct CompactSpritePair *gUnk_020004D8;
void Sprite_Remove(void *);
void sub_0813A91C(u32 index)
{
    struct CompactSpritePair *pair = gUnk_020004D8;
    if (pair && index <= 1 && pair->slots[index].active) {
        Sprite_Remove(&pair->slots[index].sprite);
        pair->slots[index].state = 0;
    }
}
