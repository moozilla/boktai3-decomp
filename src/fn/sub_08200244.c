#include "global.h"
#define DirectionalSprite_FoldFacing sub_08200244
struct DirectionalSprite {
    u8 unknown0[0x320];
    u8 foldedFacing;
    u8 facing;
    u8 mirrored;
};
void sub_08200244(struct DirectionalSprite *sprite)
{
    u8 facing = sprite->facing;
    if (facing > 4) {
        sprite->foldedFacing = 8 - facing;
        sprite->mirrored = 1;
    } else {
        sprite->foldedFacing = facing;
        sprite->mirrored = 0;
    }
}
