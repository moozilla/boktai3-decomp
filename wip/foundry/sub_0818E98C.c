#define SpriteColliderGroup_Remove sub_0818E98C
#include "global.h"
struct SpriteColliderPart {
    u8 sprite[0x9c];
    u8 collider[0x148];
};
void Sprite_Remove(void *);
void Collider_Remove(void *);
void sub_0818E98C(struct SpriteColliderPart *parts)
{
    u8 *collider = parts->collider;
    u8 *part = (u8 *)parts;
    s32 i = 2;
    do {
        Sprite_Remove(part);
        Collider_Remove(collider);
        {
            u16 stride = 0x1e4;
            collider += stride;
            part += stride;
        }
    } while (--i >= 0);
}
