#define Collider_SetParameters Collider_SetAttackProperties
#include "global.h"
struct SpriteComponent {
    u8 unknown00[0x5C];
    u8 collider[0x48];
};
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void SpriteComponent_ConfigureCollider(struct SpriteComponent *component, s32 parameter3C,
                                       s32 parameter40, s32 parameter44)
{
    Collider_SetAttackProperties(component->collider, parameter3C, parameter40, 0, 0, parameter44);
}
