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
    u32 unknown0;
    u16 tileBase;
};
struct SpriteEffectTileDrawable {
    u32 flags;
    u8 active, unknown5[3];
    u16 tile;
    u8 width, height;
    u8 unknownC[0xC];
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
extern struct SpriteEffectControllerPool *gUnk_02000178;
void sub_0806A60C(struct SpriteEffectControllerPool *);
void sub_0806A694(struct SpriteEffectController *, struct ContactPosition *);
void sub_0806A6D0(struct SpriteEffectController *, s32, s32, s32, s32);
struct SpriteEffectController *sub_0806A788(struct SpriteEffectControllerPool *);
void sub_0806A7D8(struct SpriteEffectControllerPool *, struct SpriteEffectController *);
void sub_0806AB1C(struct SpriteEffectControllerPool *, struct SpriteEffectController *);
void sub_0806AF74(struct SpriteEffectControllerPool *, struct SpriteEffectController *);
#endif
