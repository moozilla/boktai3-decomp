#include "global.h"
#include "ram.h"

/* tentative: hides the sprite and spawns three randomly scattered bursts */

struct Vec3 {
    s32 x, y, z, w;
};
struct BurstActor {
    u32 flags;
    u8 pad4[0x60 - 4];
    u32 spriteFlags;
    u8 pad64[0xA0 - 0x64];
    struct Vec3 position;
    u8 padB0[0xCA - 0xB0];
    u8 state;
    u8 active;
};
extern const u16 gUnk_086141BC[];
void Sound_PlaySong(u32);
void SpriteEffect_CreateAtPosition(void *, void *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32,
                                   u32, u32, u32);

#define RAND()                                                                                     \
    (gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023,                               \
     gUnk_086141BC[gEffectRandomTableIndex])

void SpriteEffect_HideWithScatterBurst(struct BurstActor *actor)
{
    struct Vec3 pos;
    if (actor->active) {
        actor->spriteFlags |= 1;
        actor->state = 0;
        actor->flags = 0;
        actor->active = 0;
        pos = actor->position;
        pos.x += ((RAND() & 15) - 7) << 12;
        pos.z += ((RAND() & 15) - 7) << 12;
        SpriteEffect_CreateAtPosition(&pos, 0, 0, 0x8639, 0x75f6, 0, 0, 0, 0, 0x153, 30, 0, 0, 1,
                                      0);
        pos = actor->position;
        pos.x += ((RAND() & 15) - 7) << 12;
        pos.z += ((RAND() & 15) - 7) << 12;
        SpriteEffect_CreateAtPosition(&pos, 0, 0, 0x8639, 0x75f6, 0, 0, 0, 0, 0x153, 30, 0, 0, 1,
                                      0);
        pos = actor->position;
        pos.x += ((RAND() & 15) - 7) << 12;
        pos.z += ((RAND() & 15) - 7) << 12;
        SpriteEffect_CreateAtPosition(&pos, 0, 0, 0x8639, 0x75f6, 0, 0, 0, 0, 0x153, 30, 0, 0, 1,
                                      0);
        Sound_PlaySong(0x195);
    }
}
