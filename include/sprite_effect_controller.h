#ifndef GUARD_SPRITE_EFFECT_CONTROLLER_H
#define GUARD_SPRITE_EFFECT_CONTROLLER_H
#include "contact_swarm.h"
/* Tentative independent sprite controller, global 02000178. Drawable layout
 * depends on flags bit 8: sequence sprite position1C, tile sprite position18. */
struct SpriteEffectDrawablePrefix {
    u32 flags;
    u8 active;
};
struct SpriteEffectTileResource {
    u16 unknown0;
    u8 flags, shape;
    u16 tileBase;
};
struct SpriteEffectTileDrawable {
    u32 flags;
    u8 active, unknown5[3];
    u16 tile;
    u8 width, height;
    u8 unknownC[8];
    u32 attributes; /* 0x14: packed OAM shape/size and color mode */
    struct ContactPosition position;
    u8 unknown20[8];
    const struct SpriteEffectTileResource *graphics;
};
struct SpriteEffectSequenceDrawable {
    u32 flags;
    u8 active, unknown5[0xB];
    u16 frame;
    u8 unknown12[0xA];
    struct ContactPosition position;
};
struct SpriteEffectController {
    void *drawable;
    struct DisplaySequenceState sequence;
    struct ContactPosition velocity;
    struct ContactCollider collider;
    u16 flags;
    u8 state;
    s8 index;
    u16 lifetime, unknown76, frame;
    u8 enter, colliderDelay;
};
struct SpriteEffectControllerPool {
    u8 task[0x18];
    u32 used;
    struct SpriteEffectController slots[24];
};
extern struct SpriteEffectControllerPool *gSpriteEffectControllerPool;
void SpriteEffectController_QueueColliders(struct SpriteEffectControllerPool *);
void SpriteEffectController_RefreshCollider(struct SpriteEffectController *,
                                            struct ContactPosition *);
void SpriteEffectController_InitCollider(struct SpriteEffectController *, s32, s32, s32, s32);
struct SpriteEffectController *
SpriteEffectController_AllocateSlot(struct SpriteEffectControllerPool *);
void SpriteEffectController_TickAnimation(struct SpriteEffectControllerPool *,
                                          struct SpriteEffectController *);
void sub_0806AB1C(struct SpriteEffectControllerPool *, struct SpriteEffectController *);
void sub_0806AF74(struct SpriteEffectControllerPool *, struct SpriteEffectController *);
#endif
