#include "global.h"
#include "engine/engine.h"
#include "scene/collision/packed_contact_vector.h"
void Hresvelgr_InitializeMotionCollider(u8 *p)
{
    struct PackedContactVector size, offset;
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
