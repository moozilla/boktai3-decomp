#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/video/display_sequence_step.h"
#include "objects/items/collectible.h"
#include "scene/collision/spatial_index.h"

struct Camera {
    u8 unknown0[0x44];
    s32 screenX, screenY;
};

// Per-frame update of a pooled collectible: animates, tests proximity and handles pickup hit.
void Collectible_UpdateEntry(struct Collectible *item)
{
    if (item->entered) {
        item->sprite.position.y = item->y - ((struct Camera *)gSceneCamera)->screenY;
        item->sprite.position.z = item->z;
        item->sprite.flags &= ~1;
        SpatialIndex_SetPosition(&item->pickup, item->x, item->y - (item->z + 0x60), item->z);
        SpatialIndex_SetPosition(&item->body, item->x, item->y - (item->z + 0x60), item->z);
        SpatialIndex_InsertEntry(&item->body);
        DisplaySequence_StepSprite(&item->sprite, &item->sequence);
        item->entered = 0;
    }
    item->sprite.position.x = item->x - ((struct Camera *)gSceneCamera)->screenX;
    if (Collectible_IsNearPlayer(item)) {
        struct SpatialEntry *body;
        u32 mask;
        SpatialIndex_InsertEntry(&item->pickup);
        body = &item->body;
        mask = ~1;
        body->flags &= mask;
    } else {
        struct SpatialEntry *body = &item->body;
        u32 mask = 1;
        body->flags |= mask;
    }
    if (item->body.hit == 1) {
        item->health -= item->body.kind;
        item->body.hit = 0;
        item->body.kind = 0;
        item->flash = 8;
        if (item->health <= 0) {
            item->body.flags |= 1;
            item->update = Collectible_StartDeath;
            item->entered = 1;
        } else {
            Sound_PlaySong(0x13E);
        }
    }
    if (item->flash) {
        AnimData_SelectPaletteBank(item->graphics, 0x192);
        item->flash--;
    } else {
        AnimData_SelectPaletteBank(item->graphics, 0x104);
    }
}
