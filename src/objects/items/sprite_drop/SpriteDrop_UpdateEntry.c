#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "objects/items/sprite_drop.h"
#include "engine/video/display_sequence_step.h"
#include "scene/collision/spatial_index.h"

struct Camera {
    u8 unknown0[0x44];
    s32 screenX, screenY;
};

// Updates a dropped sprite only when its manager requests an animation tick.
void SpriteDrop_UpdateEntry(struct SpriteDrop *item)
{
    if (item->entered) {
        item->sprite.position.y = item->y - ((struct Camera *)gSceneCamera)->screenY;
        item->sprite.position.z = item->z;
        item->sprite.flags &= ~1;
        SpatialIndex_SetPosition(&item->pickup, item->x, item->y - (item->z + 96), item->z);
        DisplaySequence_StepSprite(&item->sprite, &item->sequence);
        item->entered = 0;
    }
    item->sprite.position.x = item->x - ((struct Camera *)gSceneCamera)->screenX;
    if (SpriteDrop_IsNearPlayer(item))
        SpatialIndex_InsertEntry(&item->pickup);
}
