#include "global.h"
#include "engine.h"
#define Hresvelgr_InitializeMotionCollider sub_081A51B8
struct MotionPackedVector {
    s32 x : 16, y : 16, z : 16;
};
void Hresvelgr_InitializeMotionCollider(u8 *p)
{
    struct MotionPackedVector size, offset;
    size.x = 64;
    size.y = 64;
    size.z = 64;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(p + 0x60, 0, 0x4000, 0, 16, &size, &offset);
    Collider_SetBodyProperties(p + 0x60, 0, 0, 0);
    Collider_SetCallback(p + 0x60, 0, p);
}
