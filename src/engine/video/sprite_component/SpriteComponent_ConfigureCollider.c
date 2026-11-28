#include "global.h"
#include "engine/video/sprite_component.h"

void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void SpriteComponent_ConfigureCollider(struct SpriteComponent *component, s32 parameter3C,
                                       s32 parameter40, s32 parameter44)
{
    Collider_SetAttackProperties(&component->collider, parameter3C, parameter40, 0, 0, parameter44);
}
