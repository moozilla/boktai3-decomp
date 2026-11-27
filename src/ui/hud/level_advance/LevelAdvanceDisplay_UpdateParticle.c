#include "ui/hud/level_advance_display.h"
void LevelAdvanceDisplay_UpdateParticle(struct LevelAdvanceParticle *particle,
                                        const struct SpriteEffectTileResource *resource)
{
    if (++particle->frame > 15) {
        particle->sprite.flags |= 1;
        particle->active = 0;
    } else {
        struct Player *player;

        particle->offsetY += particle->velocityY;
        player = gPlayers[0];
        particle->sprite.position.x = player->position.x + particle->offsetX;
        particle->sprite.position.y = player->position.y + particle->offsetY;
        particle->sprite.position.z = player->position.z + particle->offsetZ;
        if ((particle->frame >> 2) & 1)
            SpriteDrawable_SelectTileFrame(&particle->sprite, resource, 3);
        else
            SpriteDrawable_SelectTileFrame(&particle->sprite, resource, 2);
    }
}
