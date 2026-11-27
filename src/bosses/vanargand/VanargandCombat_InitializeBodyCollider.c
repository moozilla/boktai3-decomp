#include "global.h"
#include "engine/engine.h"
#include "scene/collision/packed_contact_vector.h"
void Collider_SetBodyProperties(void *, s32, s32, s32);
void VanargandCombat_InitializeContact(void);
struct ActorStatus {
    u8 pad0[0x1C];
    u8 position[8];
    u8 pad24[0xE4];
    u8 bodyCollider[0x54];
    u32 mode;
};
void VanargandCombat_InitializeBodyCollider(struct ActorStatus *status)
{
    void *collider = status->bodyCollider;
    struct PackedContactVector size, offset;
    size.x = 0x100;
    size.y = 0x100;
    size.z = 0x100;
    offset.x = 0;
    offset.y = 0x80;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5001, 0, 0x10, &size, &offset);
    Collider_SetPosition(collider, status->position, 0);
    Collider_SetCallback(collider, VanargandCombat_InitializeContact, status);
    if (status->mode == 0)
        Collider_SetBodyProperties(collider, 100, 0x801, 2);
    else
        Collider_SetBodyProperties(collider, 100, 0x802, 1);
    Collider_Refresh(collider);
}
