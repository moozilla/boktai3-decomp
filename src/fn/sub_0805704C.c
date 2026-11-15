#include "level_advance_display.h"
void LevelAdvanceDisplay_UpdateParticle(struct LevelAdvanceParticle *particle, void *resource)
{
    if (++particle->frame > 15) {
        particle->flags |= 1;
        particle->active = 0;
    } else {
        struct LevelAdvancePlayer *player;

        particle->offsetY += particle->velocityY;
        player = gPlayers[0];
        particle->position.world.x = player->position.world.x + particle->offsetX;
        particle->position.world.y = player->position.world.y + particle->offsetY;
        particle->position.world.z = player->position.world.z + particle->offsetZ;
        if ((particle->frame >> 2) & 1)
            SpriteDrawable_SelectTileFrame(particle, resource, 3);
        else
            SpriteDrawable_SelectTileFrame(particle, resource, 2);
    }
}
