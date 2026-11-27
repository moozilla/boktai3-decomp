#include "global.h"
#include "scene/collision/contact_position.h"
#include "recovered/fragment_effect_sprite_layout.h"
struct HresvelgrMotion {
    u8 unknown0[0x18];
    struct FragmentEffectSprite sprite;
    u8 graphics[0x70];
    struct ContactPosition origin, velocity, offset;
    u8 shadow[0x6c];
    u16 state, stateFrame;
};
s32 AnimData_Load(void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Sprite_Init(void *, void *, u32);
void Hresvelgr_InitializeMotionCollider(struct HresvelgrMotion *);
void ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void ScaledShadow_SetGraphic(void *, s32, s32);
s32 Hresvelgr_InitializeFallingActor(struct HresvelgrMotion *p, struct ContactPosition *position,
                                     struct ContactPosition *velocity, u32 flip)
{
    void *graphics = p->graphics;
    struct FragmentEffectSprite *sprite;
    void *shadow;
    if (!AnimData_Load(graphics, 0x3070))
        return -1;
    sprite = &p->sprite;
    Sprite_Init(sprite, graphics, 0);
    {
        u32 value = 5;
        sprite->frame = value;
    }
    AnimData_SelectPaletteBank(graphics, 0x2f2);
    if (flip)
        p->sprite.flags |= 4;
    p->origin = *position;
    p->sprite.position = *position;
    p->sprite.priority = 2;
    p->sprite.flags |= 1;
    p->velocity = *velocity;
    Hresvelgr_InitializeMotionCollider(p);
    shadow = p->shadow;
    ScaledShadow_Initialize(shadow, &p->sprite.position, 64, 64, 64, 64, 0x1900, 1, 6, 1, 0);
    ScaledShadow_SetGraphic(shadow, 0x1c1b, 7);
    p->state = 0;
    p->stateFrame = 0;
#ifdef UBFIX
    // falls off the end with r0 left over from its last statement; the creator only tests
    // < 0, and this initializer has no failure path here
    return 0;
#endif
}
