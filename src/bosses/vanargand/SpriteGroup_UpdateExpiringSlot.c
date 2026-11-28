#include "bosses/vanargand/sprite_trail.h"
#include "scene/collision/spatial_index.h"
#include "bosses/vanargand/flag_accessors.h"

void SpriteGroup_RemoveSlot(struct SpriteTrailPool *, struct SpriteTrail *, s32);
void SpriteTrail_UpdatePositions(struct SpriteTrail *);
void SpriteGroup_UpdateExpiringSlot(struct SpriteTrailPool *group, struct SpriteTrail *slot,
                                    s32 index)
{
    if (slot->entered) {
        slot->entered = 0;
        {
            struct SpatialEntry *collider = (void *)slot->renderCollider;
            SpatialEntry_SetFlags(collider, 1);
        }
    }
    SpriteTrail_UpdatePositions(slot);
    if ((s32)slot->frame > 0)
        SpriteGroup_RemoveSlot(group, slot, index);
    else
        slot->frame++;
}
