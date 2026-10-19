#define Hresvelgr_InitializeFallingActor sub_081A522C
#define Hresvelgr_InitializeMotionCollider sub_081A51B8
#include "global.h"
struct HresvelgrPosition { s16 x,y,z,pad; };
struct HresvelgrSprite {
 u32 flags; u8 unknown4[3]; u8 priority; u8 unknown8[8];
 u16 field10; u8 unknown12[10]; struct HresvelgrPosition position;
 u8 unknown24[8];
};
struct HresvelgrMotion {
 u8 unknown0[0x18]; struct HresvelgrSprite sprite;
 u8 graphics[0x70]; struct HresvelgrPosition origin, velocity, offset;
 u8 shadow[0x6c]; u16 state, stateFrame;
};
s32 AnimData_Load(void *,u32);
void AnimData_SelectPaletteBank(void *,u32);
void Sprite_Init(void *,void *,u32);
void Hresvelgr_InitializeMotionCollider(struct HresvelgrMotion *);
void ScaledShadow_Initialize(void *,void *,s32,s32,s32,s32,s32,s32,s32,s32,s32);
void ScaledShadow_SetGraphic(void *,s32,s32);
s32 sub_081A522C(struct HresvelgrMotion *p, struct HresvelgrPosition *position, struct HresvelgrPosition *velocity, u32 flip)
{
 void *graphics = p->graphics;
 struct HresvelgrSprite *sprite;
 void *shadow;
 u32 zero;
 if (!AnimData_Load(graphics, 0x3070)) return -1;
 sprite = &p->sprite;
 Sprite_Init(sprite, graphics, 0);
 {
 u32 value = 5;
 zero = 0;
 sprite->field10 = value;
 }
 AnimData_SelectPaletteBank(graphics, 0x2f2);
 if (flip) p->sprite.flags |= 4;
 p->origin = *position;
 p->sprite.position = *position;
 p->sprite.priority = 2;
 p->sprite.flags |= 1;
 p->velocity = *velocity;
 Hresvelgr_InitializeMotionCollider(p);
 shadow = p->shadow;
 ScaledShadow_Initialize(shadow, &p->sprite.position, 64, 64, 64, 64, 0x1900, 1, 6, 1, zero);
 ScaledShadow_SetGraphic(shadow, 0x1c1b, 7);
 p->state = zero;
 p->stateFrame = zero;
}
