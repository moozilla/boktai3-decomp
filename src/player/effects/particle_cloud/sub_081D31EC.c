// confidence=low: moves the cloud anchor with the player and animates the forty particles.
#include "global.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "player/effects/player_particle_cloud.h"
#include "system/game_state/game_state.h"

extern const u16 gUnk_086126C8[];
extern void (*const gUnk_086126BC[])(struct CloudTask *, struct CloudParticle *);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);

u32 sub_081D31EC(struct CloudTask *task)
{
    struct CloudParticle *particle;
    s32 i;

    switch (task->mode) {
        case 1:
            task->anchor = task->player->position;
            task->anchor.y += 0x80;
            break;
        case 2: {
            struct CloudPlayer *player = task->player;
            u32 mask = 0x80;
            if (player->statusFlags & mask) {
                task->anchor = task->player->position;
                if (gGameState->unk85E == 0) {
                    task->anchor.x += 0x14;
                    task->anchor.y += 0x1E0;
                    task->anchor.z -= 0x14;
                } else {
                    task->anchor.x -= 0x14;
                    task->anchor.y += 0x1E0;
                    task->anchor.z += 0x14;
                }
                task->duration = 6;
            } else {
                task->anchor = task->player->position;
                task->anchor.y += 0x80;
                task->duration = 4;
            }
        } break;
        case 3:
            task->anchor = task->player->position;
            if (gGameState->unk85E == 0) {
                task->anchor.x += 0x14;
                task->anchor.y += 0x1E0;
                task->anchor.z -= 0x14;
            } else {
                task->anchor.x -= 0x14;
                task->anchor.y += 0x1E0;
                task->anchor.z += 0x14;
            }
            task->duration = 4;
            break;
    }

    particle = task->particles;
    for (i = 0; i < 40; i++, particle++) {
        if (particle->kind) {
            if (++particle->timer >= gUnk_086126C8[particle->frame]) {
                particle->timer = 0;
                if (++particle->frame > 1)
                    particle->frame = 0;
                SpriteDrawable_SelectTileFrame(&particle->sprite, task->sheet,
                                               task->frameBase + particle->frame);
            }
            gUnk_086126BC[particle->kind](task, particle);
        }
    }
    if (task->stop)
        Task_Kill(&task->task);
    task->ticks++;
    return 0;
}
