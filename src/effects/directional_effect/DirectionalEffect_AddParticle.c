#include "effects/directional_effect/spark_burst.h"

void DirectionalEffect_AddParticle(struct PlayerSparkView *player, struct ContactPosition *position,
                                   struct ContactPosition *velocity, u32 lifetime)
{
    struct DirectionalSpark *spark = &player->burst.sparks[player->burst.cursor];

    spark->drawable.flags &= ~1;
    spark->baseFrame = lifetime * 2 + 2;
    SpriteDrawable_SelectTileFrame(&spark->drawable, player->burst.resource, spark->baseFrame);
    spark->drawable.position = *position;
    spark->velocity = *velocity;
    spark->age = 0;
    spark->active = 1;
    player->burst.active = 1;
    if (++player->burst.cursor > 5)
        player->burst.cursor = 0;
}
