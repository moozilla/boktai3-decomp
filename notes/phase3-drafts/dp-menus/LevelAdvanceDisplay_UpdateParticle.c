void LevelAdvanceDisplay_UpdateParticle(struct LevelAdvanceParticle *particle, void *resource)
{
    u32 frame;
    {
        u32 value = particle->frame;
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
        player = gPlayers[0];
        particle->position.world.x = player->position.world.x + particle->offsetX;
        particle->position.world.y = player->position.world.y + y;
        particle->position.world.z = player->position.world.z + particle->offsetZ;
        {
            u32 phase = (u16)frame >> 2;
            phase &= 1;
            if (phase)
                SpriteDrawable_SelectTileFrame(particle, resource, 3);
            else
                SpriteDrawable_SelectTileFrame(particle, resource, 2);
        }
    }
}
