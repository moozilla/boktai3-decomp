#include "global.h"
struct SpriteEffectActor {
    u32 flags;
    u8 pad4[0x60-4];
    u8 sprite[0x40];
    u8 position[0x20];
    s32 height;
    u8 padC4[7];
    u8 state;
    u8 padCC[2];
    u8 active, secondary;
};
struct SpriteEffectOffset { u32 x:16, y:16, z:16, pad:16; };
void Sprite_Remove(void *);
void Sound_PlaySong(u32);
void SpriteEffect_CreateAtPosition(void *, struct SpriteEffectOffset *, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);
void SpriteEffect_RemoveWithBurst(struct SpriteEffectActor *actor)
{
    struct SpriteEffectOffset offset;
    if (actor->active) {
        Sprite_Remove(actor->sprite);
        actor->secondary = 0;
        actor->state = 0;
        actor->flags = 0;
        actor->active = 0;
        offset.x = 0;
        offset.y = actor->height >> 12;
        offset.z = 0;
        SpriteEffect_CreateAtPosition(actor->position, &offset, 0, 0x8639, 0x75f6, 0, 0, 0, 0, 0x153, 30, 0, 0, 1, 0);
        Sound_PlaySong(0x16e);
    }
}
