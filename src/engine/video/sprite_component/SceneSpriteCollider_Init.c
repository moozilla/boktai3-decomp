#include "global.h"
#include "engine/video/sprite_component.h"

struct ColliderExtent {
    u32 x : 16, y : 16, z : 16;
    u32 : 16;
};
s32 AnimData_Load(void *, u32);
void Sprite_Init(struct DisplaySequenceSprite *, void *, u32);
void AnimData_SelectPaletteBank(void *, u32);
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void SceneActorAttachment_MarkContact(void);
s32 Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
s32 SceneSpriteCollider_Init(struct SpriteComponent *component)
{
    struct DisplaySequenceSprite *sprite = &component->sprite;
    struct ContactCollider *collider;
    struct ColliderExtent size, offset;
    u32 initialFrame;
    s32 result;

    AnimData_Load(&component->resource, 0x2110);
    Sprite_Init(sprite, &component->resource, 0x43);
    initialFrame = 4;
    sprite->frame = initialFrame;
    AnimData_SelectPaletteBank(&component->resource, 0x51);
    sprite->priority = 1;
    sprite->paletteVariant = 0x7f;
    sprite->palette = 0x7f;
    ZeroFill8(&sprite->position);
    collider = &component->collider;
    size.x = 100;
    size.y = 100;
    size.z = 100;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 0x10, &size, &offset);
    Collider_SetPosition(collider, &sprite->position, 0);
    Collider_SetCallback(collider, SceneActorAttachment_MarkContact, component);
    result = Collider_SetAttackProperties(collider, 30, 10, 0, 2, 10);
    component->collisionStatus = 0;
    return result;
}
