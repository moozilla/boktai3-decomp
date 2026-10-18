#include "global.h"
/* Tentative family: the drawable at 0x358 attached to the actor. */
struct ActorOverlaySprite {
    u32 flags;
    u8 unknown4[12];
    u16 palette;
    u8 unknown12[0x22];
};
struct ActorOverlayCallbacks {
    void (*update)(void);
    void (*draw)(void);
};
struct ActorOverlay {
    u8 unknown0[0xD6];
    u16 sourceId;
    u8 unknownD8[0x224];
    u8 resource[0x54];
    struct ActorOverlayCallbacks callbacks;
    struct ActorOverlaySprite sprite;
    u16 id;
    u8 state;
    u8 unknown38F[0x7A9];
    u8 variant;
};
void Sprite_Init(void *, void *, u32);
void SceneParticle_UpdateBounce(void);
void BouncingEffect_RemoveSprite(void);
extern const u8 gUnk_0824E6C4[];
void ActorOverlay_InitSprite(struct ActorOverlay *p)
{
    struct ActorOverlaySprite *sprite = &p->sprite;
    Sprite_Init(sprite, p->resource, 0);
    sprite->flags = 0x202;
    {
        struct ActorOverlayCallbacks *callbacks = &p->callbacks;
        void (*update)(void) = SceneParticle_UpdateBounce;
        void (*draw)(void) = BouncingEffect_RemoveSprite;
        callbacks->update = update;
        callbacks->draw = draw;
    }
    p->state = 0;
    p->id = p->sourceId;
    sprite->palette = gUnk_0824E6C4[p->variant];
}
