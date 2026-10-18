#define Hresvelgr_InitializeCollider sub_0819C028
#define Hresvelgr_ColliderNoOp       sub_0819C024
#include "global.h"
#include "engine.h"
#include "scene_vec.h"
struct Hresvelgr {
    u8 unknown0[0x1B4];
    struct SceneVec colliderPosition;
    u8 unknown1BC[0x38];
    u8 collider[0x40];
    u8 unknown234[0x124];
    s16 attackPower, attackForce, attackParameter;
};
typedef struct {
    s16 x, y, z;
} HresvelgrColliderVector;
void sub_0819C024(void);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void sub_0819C028(struct Hresvelgr *actor, u16 id)
{
    HresvelgrColliderVector scale, rotation;
    void *collider = actor->collider;
    scale.x = 0x40;
    scale.y = 0x80;
    scale.z = 0x40;
    rotation.x = 0;
    rotation.y = 0x80;
    rotation.z = 0;
    Collider_Init(collider, id, 0x2001, 0, 16, &scale, &rotation);
    Collider_SetPosition(collider, &actor->colliderPosition, 0);
    Collider_SetCallback(collider, sub_0819C024, actor);
    Collider_SetAttackProperties(collider, actor->attackPower, actor->attackForce, 0, 0,
                                 actor->attackParameter);
}
