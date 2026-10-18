#include "global.h"
#define ActorCollider_Init sub_081213B4
struct ColliderExtentPair {
    u32 lo : 16, hi : 16;
};
struct ColliderExtentTail {
    u32 lo : 16;
    u32 : 16;
};
struct ColliderExtents {
    struct ColliderExtentPair a;
    struct ColliderExtentTail b;
};
struct ColliderObject {
    u8 unknown0[0x1C];
    u8 position[0x28];
    u8 collider[0x54];
};
void Collider_Init(void *, u32, u32, u32, u32, struct ColliderExtents *, struct ColliderExtents *);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void Collider_SetCallback(void *, void (*)(void), void *);
void Collider_SetPosition(void *, void *, u32);
void SceneColliderEffect_OnCollision(void);
void sub_081213B4(struct ColliderObject *object, u32 a, u32 b, u32 value)
{
    struct ColliderExtents size, offset;
    void *collider;
    size.a.lo = 16;
    size.a.hi = 256;
    size.b.lo = 16;
    offset.a.lo = 0;
    offset.a.hi = 128;
    offset.b.lo = 0;
    collider = object->collider;
    Collider_Init(collider, 0, 0x2101, 0, 16, &size, &offset);
    Collider_SetAttackProperties(collider, a, b, 0, 0x8020, value);
    Collider_SetCallback(collider, SceneColliderEffect_OnCollision, object);
    Collider_SetPosition(collider, object->position, 0);
}
