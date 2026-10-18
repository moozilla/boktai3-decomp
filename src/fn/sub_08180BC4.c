#define gActorMarkerFacingFrames gUnk_0824E6B4
#include "global.h"

struct ActorMarkerSprite {
    u32 flags;
    u8 pad4[12];
    u16 frame;
};
struct ActorMarker {
    u8 pad0[0xd6];
    u16 facingValue;
    u8 padD8[0x334 - 0xd8];
    u8 animation[0x3d0 - 0x334];
    struct {
        void (*update)(void *);
        void (*draw)(void *);
    } callbacks;
    struct ActorMarkerSprite sprite;
    u8 pad3EC[0x40c - 0x3ec];
    u16 savedFacing;
    u8 state;
    u8 pad40F[0xb38 - 0x40f];
    u8 kind, direction;
};
void Sprite_Init(void *, void *, u32);
void SceneParticle_UpdateBounce(void *);
void BouncingEffect_RemoveSprite(void *);
extern const u8 gUnk_0824E6B4[];

void ActorMarker_InitFacingSprite(struct ActorMarker *p)
{
    struct ActorMarkerSprite *sprite = &p->sprite;
    s32 direction;
    u32 frame;
    Sprite_Init(sprite, p->animation, 0);
    sprite->flags = 0x202;
    {
        typeof(p->callbacks) *callbacks = &p->callbacks;
        void (*update)(void *) = SceneParticle_UpdateBounce;
        void (*draw)(void *) = BouncingEffect_RemoveSprite;
        callbacks->update = update;
        callbacks->draw = draw;
    }
    p->state = 0;
    p->savedFacing = p->facingValue;
    frame = gUnk_0824E6B4[p->kind];
    direction = ((p->direction + 0x60) >> 6) & 3;
    switch (direction) {
        case 0:
            sprite->frame = frame;
            break;
        case 1:
            sprite->frame = frame - 4;
            break;
        case 2:
            sprite->frame = frame - 4;
            sprite->flags |= 4;
            break;
        case 3:
            sprite->frame = frame;
            sprite->flags |= 4;
            break;
    }
}
