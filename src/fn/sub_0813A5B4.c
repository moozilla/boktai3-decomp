#include "global.h"
#include "ram.h"
#define gSpritePair gSpritePairPool
struct SpritePairSlot {
    u32 callback;
    u8 pad04[0x5c];
    u32 sprite;
    u8 active;
    u8 pad65[0x69];
    u8 state;
    u8 padCF;
    u16 timer;
    u8 padD2[0xa];
};
struct SpritePair {
    u8 pad00[0x18];
    struct SpritePairSlot slots[2];
};
void Sprite_Remove(void *);
void SpritePair_ReleaseSlot(u32 index, u32 remove)
{
    struct SpritePair *pair = gSpritePairPool;
    if (pair && index <= 1 && pair->slots[index].active) {
        pair->slots[index].state = 0;
        pair->slots[index].timer = 0;
        pair->slots[index].callback = 0;
        if (remove == 1)
            Sprite_Remove(&pair->slots[index].sprite);
        else
            pair->slots[index].sprite |= 1;
    }
}
