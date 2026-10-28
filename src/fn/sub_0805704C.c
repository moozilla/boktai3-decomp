// MATCHING_CONCESSION: register-pinning
#include "level_advance_display.h"
void LevelAdvanceDisplay_UpdateParticle(struct LevelAdvanceParticle *input, void *resourceInput)
{
    struct LevelAdvanceParticle *particle = input;
    void *resource = resourceInput;
    register u32 frame asm("r4");
    {
        register u32 value asm("r0") = particle->frame;
        frame = value + 1;
    }
    particle->frame = frame;
    if ((u16)frame > 15) {
        particle->flags |= 1;
        particle->active = 0;
    } else {
        u32 y;
        struct LevelAdvancePlayer *player;
        {
            u32 velocity = (u16)particle->velocityY;
            u32 offset = (u16)particle->offsetY;
            y = velocity + offset;
        }
        particle->offsetY = y;
        {
            register u32 x asm("r0");
            register u32 playerX asm("r6");
            player = gPlayers[0];
            x = (u16)particle->offsetX;
            playerX = (u16)player->position.world.x;
            particle->position.world.x = x + playerX;
        }
        particle->position.world.y = player->position.world.y + y;
        {
            register u32 z asm("r0") = (u16)particle->offsetZ;
            register u32 playerZ asm("r2") = (u16)player->position.world.z;
            particle->position.world.z = z + playerZ;
        }
        {
            register u32 phase asm("r0") = (u16)frame >> 2;
            phase &= 1;
            if (phase)
                SpriteDrawable_SelectTileFrame(particle, resource, 3);
            else
                SpriteDrawable_SelectTileFrame(particle, resource, 2);
        }
    }
}
