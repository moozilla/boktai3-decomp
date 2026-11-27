#include "bosses/vanargand/vanargand_damage_part.h"
#include "scene/collision/packed_contact_halfwords.h"
#include "scene/collision/packed_contact_vector.h"
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
s32 Collider_Refresh(void *);
void Collider_SetBodyProperties(void *, s32, s32, s32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Vanargand_OnPartContact(void);
void Vanargand_InitPartCollider(struct VanargandDamagePart *part)
{
    struct PackedContactVector size, offset;
    struct ContactCollider *collider = &part->collider;
    size.x = 128;
    size.y = 256;
    size.z = 128;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(collider, 0, 0x5001, 0, 16, &size, &offset);
    Collider_SetPosition(collider, &part->sprite.position, 0);
    Collider_SetCallback(collider, Vanargand_OnPartContact, part);
    Collider_SetBodyProperties(collider, 100, 2, 1);
    {
        u32 flags = 32;
        part->collider.direction = flags;
    }
    Collider_Refresh(collider);
}
