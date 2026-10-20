#include "contact_actor.h"
#define SceneScriptObject_InitHitCollider sub_081B85E0
void SceneScriptObject_ApplyHitFlash(void);
void SceneScriptObject_InitHitCollider(u8 *p)
{
    void *collider = p + 0x94;
    struct ContactPackedVector bounds, offset;
    bounds.x = 96;
    bounds.y = 128;
    bounds.z = 96;
    offset.x = 0;
    offset.y = 64;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5001, 0, 16, &bounds, &offset);
    Collider_SetPosition(collider, p + 0x34, 0);
    Collider_SetCallback(collider, SceneScriptObject_ApplyHitFlash, p);
    Collider_SetBodyProperties(collider, 0, 0, 0);
    Collider_Refresh(collider);
}
