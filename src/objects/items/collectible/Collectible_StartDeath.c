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
s32 Terrain_GetCellClearance(s16 *);
void Sprite_BlinkUntilExpired(void *, u32, u32, u32);

// Collectible defeat: launches a bouncing death animation, then expires.
void Collectible_StartDeath(struct Collectible *item)
{
    s32 clearance;
    if (item->entered) {
        u32 sequence, flip;
        item->velocityX = 0x60;
        item->velocityY = -80;
        item->entered = 0;
        switch (item->type) {
            case 0:
                sequence = 5;
                flip = 0;
                Sound_PlaySong(0x8D);
                break;
            case 2:
                sequence = 5;
                flip = 1;
                Sound_PlaySong(0x8D);
                break;
            case 1:
                sequence = 3;
                flip = 1;
                Sound_PlaySong(0x3E7);
                break;
            default:
                sequence = 0;
                flip = 0;
                break;
        }
        Anim_SetSequence(&item->sequence, item->sequences, (u8)sequence, (u8)flip, 0);
        DisplaySequence_StepSprite(&item->sprite, &item->sequence);
        item->timer = 0;
        SpatialIndex_RemoveEntry(&item->body);
        ActorParticles_SpawnAlternatingBurst(item);
        AnimData_SelectPaletteBank(item->graphics, 0x104);
    }
    item->x += item->velocityX >> 4;
    item->y += item->velocityY >> 4;
    item->velocityY += 4;
    clearance = Terrain_GetCellClearance(&item->x);
    if (clearance < item->y) {
        item->y = clearance;
        item->velocityY = Div(-item->velocityY * 4, 10);
        item->velocityX = Div(item->velocityY * 4, 10);
    }
    item->sprite.position.x = item->x - ((struct Camera *)gSceneCamera)->screenX;
    item->sprite.position.y = item->y - ((struct Camera *)gSceneCamera)->screenY;
    if (item->timer > 30)
        Sprite_BlinkUntilExpired(&item->sprite, item->timer, 50, 80);
    if (item->timer > 100) {
        item->update = Collectible_OnDeactivate;
        item->entered = 1;
    } else {
        item->timer++;
    }
}
